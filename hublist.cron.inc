<?php

function _hublist_cron_pinger() {
  if (_hublist_include('hublist', 'hublist.pinger.nmdc.inc') && _hublist_include('hublist', 'hublist.pinger.adc.inc')) {
    if (variable_get('hublist_pinger_enabled', FALSE)) {
      $default_pinger_nick = variable_get('hublist_pinger_nick', '');
      $default_pinger_password = variable_get('hublist_pinger_password', '');
      $maximum_offline_counter = variable_get('hublist_pinger_maximum_offline_counter', 100);

      $sql = "SELECT nid, vid, hub_protocol, hub_address, pinger_nick, pinger_password, pinger_share, pinger_slots, stat_successful_pings, stat_all_pings, stat_offline_count, hub_status, stat_pingdate ";
      $sql .= "FROM {hublist} ";
      $sql .= "WHERE vid in(SELECT MAX(vid) from {hublist} GROUP BY nid) AND hub_status < 2 ORDER BY stat_pingdate ASC LIMIT %d, %d";

      $hubs = db_query($sql, 0, variable_get('hublist_pinger_hub_count', '10'));
      while ($hub = db_fetch_array($hubs)) {
        $hub_address = explode(':', $hub['hub_address']);

        if (sizeof($hub_address) > 0) {
          $hub_host = $hub_address[0];
        }
        else {
          $hub_host = "";
        }

        if (sizeof($hub_address) > 1) {
          $hub_port = $hub_address[1];
        }
        else {
          $hub_port = 0;
        }

        if (isset($hub['pinger_nick']) and strlen($hub['pinger_nick'])) {
          $pinger_nick = $hub['pinger_nick'];
        }
        else {
          $pinger_nick = $default_pinger_nick;
        }

        if (isset($hub['pinger_password']) and strlen($hub['pinger_password'])) {
          $pinger_password = $hub['pinger_password'];
        }
        else {
          $pinger_password = $default_pinger_password;
        }

        // add 1 byte to be sure that we are _over_ the sharelimit
        $pinger_share = $hub['pinger_share'] * 1073741824 + 1;
        $pinger_share = (string)$pinger_share;

        $pinger_slots = (string)$hub['pinger_slots'];

        switch ($hub['hub_protocol']) {
        case 'adc://':
          $pinger = new AdcPinger();
          $result = $pinger->ping($hub_host, $hub_port, $pinger_nick, $pinger_password, $pinger_share, $pinger_slots, FALSE);
          break;

        case 'adcs://':
          $pinger = new AdcPinger();
          $result = $pinger->ping($hub_host, $hub_port, $pinger_nick, $pinger_password, $pinger_share, $pinger_slots, TRUE);
          break;

        case 'dchub://':
          $pinger = new NmdcPinger();
          $result = $pinger->ping($hub_host, $hub_port, $pinger_nick, $pinger_password, $pinger_share, $pinger_slots, FALSE);
          break;
        }

        $status_message = '';
        if ($result['error'] != '') {
          watchdog('pinger', 'Checking @huburl failed: @errormsg', array('@huburl' => $hub['hub_protocol'] . $hub['hub_address'], '@errormsg' => $result['error']), WATCHDOG_ERROR, l(t('view'), 'node/' . $hub['nid']));
          $status_message = $result['error'];
        }
        elseif ($result['warning'] != '') {
          watchdog('pinger', 'A problem occured while checking @huburl: @errormsg', array('@huburl' => $hub['hub_protocol'] . $hub['hub_address'], '@errormsg' => $result['warning']), WATCHDOG_WARNING, l(t('view'), 'node/' . $hub['nid']));
          $status_message = $result['warning'];
        }
        elseif (!$result['pingerextension']) {
          $status_message = 'Pinger protocol extension is not supported, hub details may be not accurate';
        }

        $sql = "UPDATE {hublist} ";
        $sql .= "SET stat_usercount = %d, stat_shared = %d, stat_shared_formatted = '%s', hub_status = %d, stat_pingdate = '%s', hub_status_comment = '%s', hub_description = '%s', stat_userlimit = %d, stat_minshare = %d, stat_minshare_formatted = '%s', stat_minslots = %d, stat_maxhubs = %d, stat_hubsoftware = '%s' ";
        $sql .= "WHERE nid = %d AND vid = %d";

        db_query($sql, $result['users'], (int)$result['shared'], _hublist_format_share($result['shared']), ($result['online'] ? 0 : 1), date('Y-m-d H:i:s', $result['date']), $status_message, $result['description'], (int)$result['userlimit'], (int)$result['minshare'], _hublist_format_share($result['minshare']), (int)$result['minslots'], (int)$result['maxhubs'], $result['hubsoftware'], $hub['nid'], $hub['vid']);

        // Update title only if we have one
        if ($result['name'] != '') {
          db_query("UPDATE {node} SET title = '%s' WHERE nid = %d AND vid = %d", $result['name'], $hub['nid'], $hub['vid']);
          db_query("UPDATE {node_revisions} SET title = '%s' WHERE nid = %d AND vid = %d", $result['name'], $hub['nid'], $hub['vid']);
        }

        // Calculate reliability
        // First cast the values to int to ensure uninitialized fields become 0
        $successful_pings = (int)$hub['stat_successful_pings'];
        $all_pings = (int)$hub['stat_all_pings'];
        $offline_counter = (int)$hub['stat_offline_count'];
        // Increment values
        $all_pings = $all_pings + 1;
        if ($result['online']) {
          $successful_pings = $successful_pings + 1;
          $offline_counter = 0;
        }
        else {
          $offline_counter = $offline_counter + 1;
          // Put the hub to inactive state if it's offline for too many times in a line
          if (($maximum_offline_counter != 0) && ($offline_counter >= $maximum_offline_counter)) {
            $status_inactive = 3;
            $status_comment = 'Offline too many times';
            db_query("UPDATE {hublist} SET hub_status = %d, hub_status_comment = '%s' WHERE nid = %d AND vid = %d", $status_inactive, $status_comment, $hub['nid'], $hub['vid']);
          }
        }
        // in theory, all_pings can't be 0, but a moron can edit the node manually to be -1 and that would cause an error at next ping
        $reliability = ($all_pings == 0) ? 'N/A' : round($successful_pings * 100 / $all_pings ) . ' % (' . $successful_pings . '/' . $all_pings . ')';

        // Update reliability statistics
        db_query("UPDATE {hublist} SET stat_all_pings = %d, stat_successful_pings = %d, stat_offline_count = %d, stat_reliability = '%s' WHERE nid = %d AND vid = %d", $all_pings, $successful_pings, $offline_counter, $reliability, $hub['nid'], $hub['vid']);

        // Try to find dupe for the current hub
        if (variable_get('hublist_filter_dupes', FALSE)) {
          $dpquery = "SELECT node.nid, node.vid, hub.hub_url FROM {node} as node INNER JOIN {hublist} as hub ON node.vid = hub.vid AND node.nid = hub.nid ";
          $dpquery .= "WHERE hub.vid in(SELECT MAX(vid) from {hublist} GROUP BY nid) ";
          $dpquery .= "AND hub.hub_status = 0 AND hub.hub_description = '%s' AND node.title = '%s' ";
          $dpquery .= "AND node.created < (SELECT created FROM {node} WHERE nid = %d AND vid = %d)";

          $similarities = db_query($dpquery, $result['description'], $result['name'], $hub['nid'], $hub['vid']);
          if (db_affected_rows() != 0) {
            $same_message = "This hub is a duplicate of the following hub(s):";
            $first = TRUE;
            while ($similar_hub = db_fetch_array($similarities)) {
              if ($first) {
                $first = FALSE;
              }
              else {
                $same_message .= ",";
              }

              $same_message .= " " . $similar_hub['hub_url'];
            }
            // Deactivating duplicate
            $status_duplicate = 4;
            db_query("UPDATE {hublist} SET hub_status = %d, hub_status_comment = '%s' WHERE nid = %d AND vid = %d", $status_duplicate, $same_message, $hub['nid'], $hub['vid']);
          }
        }
      }
    }
  }
}

function _hublist_format_share($share) {
  if ($share >= 1024*1024*1024*1024) {
    return round($share / (1024*1024*1024*1024), 2) . ' TiB';
  }
  elseif ($share >= 1024*1024*1024) {
    return round($share / (1024*1024*1024), 2) . ' GiB';
  }
  elseif ($share >= 1024*1024) {
    return round($share / (1024*1024), 2) . ' MiB';
  }
  elseif ($share >= 1024) {
    return round($share / 1024, 2) . ' KiB';
  }
  else {
    return $share . ' B';
  }
}

// This function converts php errors to exceptions
// Used by :ping() to catch pinger exceptions
function _hublist_handle_error($errno, $errstr, $errfile, $errline, array $errcontext) {
    // error was suppressed with the @-operator
    if (0 === error_reporting()) {
        return FALSE;
    }
    throw new ErrorException($errstr, 0, $errno, $errfile, $errline);
}

function _hublist_cron_generator() {
  if (variable_get('hublist_generator_enabled', FALSE)) {
    // The last generation of the xml list in unix time
    $xmllastgenerated = variable_get('hublist_generator_xmllastgenerated', 0);
    // The generation interval in minutes. 0 = disabled
    $xmlinterval = variable_get('hublist_generator_xmlinterval', 0);

    // Generate list only if it's enabled and we passed the required time
    if (($xmlinterval >= 0) and ($xmllastgenerated + $xmlinterval * 60 <= time())) {
      // First we store the last generation attempt
      $xmllastgenerated = time();
      variable_set('hublist_generator_xmllastgenerated', $xmllastgenerated);

      $hublist_name = variable_get('hublist_generator_name', '');
      $hublist_url = variable_get('hublist_generator_url', '');
      $hublist_description = variable_get('hublist_generator_description', '');

      // Creating new XML hublist
      $xml = new DomDocument('1.0', 'UTF-8');
      $xml->xmlStandalone = TRUE;
      $xml->formatOutput = TRUE;

      // Creating "Hublist" root element
      $root = $xml->createElement('Hublist');
      $xml->appendChild($root);

      // Adding "Hubs" Child element
      $hubs = $xml->createElement('Hubs');
      $root->appendChild($hubs);

      // Adding "Columns"
      $columns = $xml->createElement('Columns');
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Name');
      $column->setAttribute('Type', 'string');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Address');
      $column->setAttribute('Type', 'string');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Description');
      $column->setAttribute('Type', 'string');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Country');
      $column->setAttribute('Type', 'string');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Users');
      $column->setAttribute('Type', 'int');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Shared');
      $column->setAttribute('Type', 'bytes');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Minshare');
      $column->setAttribute('Type', 'bytes');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Minslots');
      $column->setAttribute('Type', 'int');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Maxhubs');
      $column->setAttribute('Type', 'int');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Maxusers');
      $column->setAttribute('Type', 'int');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Reliability');
      $column->setAttribute('Type', 'string');
      $columns->appendChild($column);
      $column = $xml->createElement('Column');
      $column->setAttribute('Name', 'Rating');
      $column->setAttribute('Type', 'string');
      $columns->appendChild($column);
      $hubs->appendChild($columns);

      // Query hubs from DB
      $sql = "SELECT node.title, hub.hub_url, hub.hub_description, hub.stat_country, hub.stat_usercount, hub.stat_minshare, hub.stat_userlimit, hub.stat_shared, hub.stat_minslots, hub.stat_maxhubs, hub.stat_successful_pings, hub.stat_all_pings ";
      $sql .= "FROM {node} AS node INNER JOIN {hublist} AS hub ON node.vid = hub.vid AND node.nid = hub.nid ";
      $sql .= "WHERE hub.vid in(SELECT MAX(vid) from {hublist} GROUP BY nid) AND hub.hub_status = 0";

      $allhubdata = db_query($sql);
      // Adding hubs one by one
      while ($hubdata = db_fetch_array($allhubdata)) {
        $hub = $xml->createElement('Hub');
        $hub->setAttribute('Name', $hubdata['title']);
        $hub->setAttribute('Address', $hubdata['hub_url']);
        $hub->setAttribute('Description', $hubdata['hub_description']);
        $hub->setAttribute('Country', $hubdata['stat_country']);
        $hub->setAttribute('Users', $hubdata['stat_usercount']);
        $hub->setAttribute('Shared', $hubdata['stat_shared']);
        $hub->setAttribute('Minshare', $hubdata['stat_minshare']);
        $hub->setAttribute('Minslots', $hubdata['stat_minslots']);
        $hub->setAttribute('Maxhubs', $hubdata['stat_maxhubs']);
        $hub->setAttribute('Maxusers', $hubdata['stat_userlimit']);
        $percentage = ($hubdata['stat_all_pings'] == 0) ? 'N/A' : round($hubdata['stat_successful_pings'] * 100 / $hubdata['stat_all_pings'], 2);
        $hub->setAttribute('Reliability', $percentage);
        // $hub->setAttribute('Rating', $hubdata['field_pingdate_value']);
        $hubs->appendChild($hub);
      }

      // Adding hublist ad
      $hub = $xml->createElement('Hub');
      $hub->setAttribute('Name', $hublist_name);
      $hub->setAttribute('Address', $hublist_url);
      $hub->setAttribute('Description', $hublist_description . ' [' . date('Y-m-d H:i:s', $xmllastgenerated) . ']');
      $hub->setAttribute('Country', "");
      $hub->setAttribute('Users', -1);
      $hub->setAttribute('Shared', 0);
      $hub->setAttribute('Rating', "");
      $hubs->appendChild($hub);

      // Getting the hublist file location and save the bzipped file
      $hublist_file = variable_get('hublist_generator_path', '');
      if ($hublist_file != '') {
        $hublist_file_handler = bzopen($hublist_file, 'w');
        bzwrite($hublist_file_handler, $xml->saveXML());
        bzclose($hublist_file_handler);
      }
    }
  }
}
