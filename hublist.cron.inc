<?php

class HublistPinger {

  public static function run() {
    if (variable_get('hublist_pinger_enabled', FALSE)) {
      $default_pinger_nick = variable_get('hublist_pinger_nick', '');
      $default_pinger_password = variable_get('hublist_pinger_password', '');
      $maximum_offline_counter = variable_get('hublist_pinger_maximum_offline_counter', 100);
      // TODO PUT BACK variable_get('hublist_pinger_hub_count', '10') INSTEAD OF 10
      $hubs = db_query("SELECT nid, vid, hub_protocol, hub_address, pinger_nick, pinger_password, pinger_share, pinger_slots, stat_successful_pings, stat_all_pings, stat_offline_count, hub_status, stat_pingdate FROM {hublist} WHERE vid in(SELECT MAX(vid) from {hublist} GROUP BY nid) AND hub_status < 1 ORDER BY stat_pingdate ASC LIMIT 0, 10");

      // $hubs = db_query("SELECT nid, vid, hub_protocol, hub_address, pinger_nick, pinger_password, pinger_share, pinger_slots, stat_successful_pings, stat_all_pings, stat_offline_count, hub_status, stat_pingdate FROM {hublist} WHERE vid in(SELECT MAX(vid) from {hublist} GROUP BY nid) AND hub_status < :hub_status ORDER BY stat_pingdate ASC LIMIT :bottom_limit, :upper_limit", array(':hub_status' => 2, ':bottom_limit' => 0, ':upper_limit' => 10));
      while ($hub = $hubs->fetchAssoc()) {
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
        $pinger_share = (string) $pinger_share;

        $pinger_slots = (string) $hub['pinger_slots'];

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

        db_update('hublist')
          ->fields(array(
            'stat_usercount' => $result['users'],
            'stat_shared' => (int) $result['shared'],
            'stat_shared_formatted' => Util::formatBytes($result['shared']),
            'hub_status' => ($result['online'] ? 0 : 1),
            'stat_pingdate' => date('Y-m-d H:i:s', $result['date']),
            'hub_status_comment' => $status_message,
            'hub_description' => $result['description'],
            'stat_userlimit' => (int) $result['userlimit'],
            'stat_minshare' => (int) $result['minshare'],
            'stat_minshare_formatted' => Util::formatBytes($result['minshare']),
            'stat_minslots' => (int) $result['minslots'],
            'stat_maxhubs' => (int) $result['maxhubs'],
            'stat_hubsoftware' => $result['hubsoftware'],
            ))
          ->condition('nid', $hub['nid'])
          ->condition('vid', $hub['vid'])
          ->execute();

        // Update title only if we have one
        if ($result['name'] != '') {
          db_update('node')
            ->fields(array(
              'title' => $result['name'],
            ))
            ->condition('nid', $hub['nid'])
            ->condition('vid', $hub['vid'])
            ->execute();
          db_update('node_revision')
            ->fields(array(
              'title' => $result['name'],
            ))
            ->condition('nid', $hub['nid'])
            ->condition('vid', $hub['vid'])
            ->execute();
        }

        // Calculate reliability
        // First cast the values to int to ensure uninitialized fields become 0
        $successful_pings = (int) $hub['stat_successful_pings'];
        $all_pings = (int) $hub['stat_all_pings'];
        $offline_counter = (int) $hub['stat_offline_count'];
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
            db_update('hublist')
              ->fields(array(
                'hub_status' => $status_inactive,
                'hub_status_comment' => $status_comment,
              ))
              ->condition('nid', $hub['nid'])
              ->condition('vid', $hub['vid'])
              ->execute();
          }
        }
        // in theory, $all_pings can't be 0, but a moron can edit the node manually to be -1 and that would cause an error at next ping
        $reliability = ($all_pings == 0) ? 'N/A' : round($successful_pings * 100 / $all_pings ) . ' % (' . $successful_pings . '/' . $all_pings . ')';

        // Update reliability statistics
        db_update('hublist')
          ->fields(array(
            'stat_all_pings' => $all_pings,
            'stat_successful_pings' => $successful_pings,
            'stat_offline_count' => $offline_counter,
            'stat_reliability' => $reliability,
          ))
          ->condition('nid', $hub['nid'])
          ->condition('vid', $hub['vid'])
          ->execute();

        // Try to find dupe for the current hub
        if (variable_get('hublist_filter_dupes', FALSE)) {

          $similarities = db_query("SELECT node.nid, node.vid, hub.hub_url FROM {node} as node INNER JOIN {hublist} as hub ON node.vid = hub.vid AND node.nid = hub.nid WHERE hub.vid in(SELECT MAX(vid) from {hublist} GROUP BY nid) AND hub.hub_status = :hub.hub_status AND hub.hub_description = :hub.hub_description AND node.title = :node.title AND node.created < (SELECT created FROM {node} WHERE nid = :nid AND vid = :vid)", array(':hub.hub_status' => 0, ':hub.hub_description' => $result['description'], ':node.title' => $result['name']));
          if ($similarities->rowCount() != 0) {
            $same_message = "This hub is a duplicate of the following hub(s):";
            $first = TRUE;
            while ($similar_hub = $similarities->fetchAssoc()) {
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
            db_update('hublist')
              ->fields(array(
                'hub_status' => $status_duplicate,
                'hub_status_comment' => $same_message,
              ))
              ->condition('nid', $hub['nid'])
              ->condition('vid', $hub['vid'])
              ->execute();
          }
        }
      } // end while
    }
  } // end method run

  public static function generateXml() {
    if (variable_get('hublist_generator_enabled', FALSE)) {
      // The last generation of the xml list in unix time
      $xmllastgenerated = variable_get('hublist_generator_xmllastgenerated', 0);
      // The generation interval in minutes. 0 = disabled
      $xmlinterval = variable_get('hublist_generator_xmlinterval', 0);

      // Generate list only if it's enabled and we passed the required time
      if (($xmlinterval >= 0) and ($xmllastgenerated + $xmlinterval * 60 <= REQUEST_TIME)) {
        // First we store the last generation attempt
        $xmllastgenerated = REQUEST_TIME;
        variable_set('hublist_generator_xmllastgenerated', $xmllastgenerated);

        $hublist_name = variable_get('hublist_generator_name', '');
        $hublist_url = variable_get('hublist_generator_url', '');
        $hublist_description = variable_get('hublist_generator_description', '');

        // Creating new XML hublist
        $xml = new DomDocument('1.0', 'UTF-8');
        $xml->xmlStandalone = TRUE;
        $xml->formatOutput = FALSE;

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
        $allhubdata = db_query("SELECT node.title, hub.hub_url, hub.hub_description, hub.stat_country, hub.stat_usercount, hub.stat_minshare, hub.stat_userlimit, hub.stat_shared, hub.stat_minslots, hub.stat_maxhubs, hub.stat_successful_pings, hub.stat_all_pings FROM {node} AS node INNER JOIN {hublist} AS hub ON node.vid = hub.vid AND node.nid = hub.nid WHERE hub.vid in(SELECT MAX(vid) from {hublist} GROUP BY nid) AND hub.hub_status = 0");

        // Adding hubs one by one
        while ($hubdata = $allhubdata->fetchAssoc()) {
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
      } // end if
    } // end if
  } // end function generateXml

} // end class HublistPinger