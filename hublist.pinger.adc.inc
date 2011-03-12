<?php

class AdcPinger {
  private $cache, $info, $sid, $socket;

  public function ping($host, $port, $nick, $password, $share = '0', $slots = '1', $secure = FALSE, $timeout = 30) {

    $this->info = array(
      'address' => gethostbyname($host),
      'country' => '',
      'date' => time(),
      'description' => '',
      'error' => '',
      'host' => $host,
      'name' => '',
      'nick' => $nick,
      'password' => $password,
      'pingershare' => $share,
      'pingerslots' => $slots,
      'port' => $port,
      'protocol' => $secure ? 'adcs://' : 'adc://',
      'shared' => 0,
      'online' => FALSE,
      'pingerextension' => TRUE, // TODO: we have to set it FALSE here then check for the existence of the PING extension later
      'users' => 0,
      'warning' => '',
      'userlimit' => 0,
      'minshare' => 0,
      'minslots' => 0,
      'maxhubs' => 0,
      'hubsoftware' => '',
    );

    echo '<span>Connecting to ' . $this->info['protocol'] . $this->info['host'] . ':' . $this->info['port'] . '...</span><br />';

    $this->socket = fsockopen(($secure ? 'ssl://' : '') . $this->info['host'], $this->info['port'], $error_number, $error_message, 5);
    if ($this->socket) {
      // Set stream timeout
      // TODO: Nem sok a 15 másodperc?
      stream_set_timeout($this->socket, 15);

      fwrite($this->socket, 'HSUP ADBAS0 ADBASE ADTIGR ADPING' . chr(10));

      while (!feof($this->socket) && !$this->info['online'] && $this->info['error'] == '') {
        $this->cache .= fread($this->socket, 2048);

        $streamstatus = stream_get_meta_data($this->socket);
        if ($streamstatus['timed_out']) {
          if (!$this->info['online']) {
            $this->info['error'] = 'Timeout on reading';
          }
        }
        else {

          if ($this->cache != '') {
            $commands = explode(chr(10), $this->cache);

            if ($this->cache[strlen($this->cache)-1] == chr(10)) {
              $command_count = sizeof($commands);
              $this->cache = '';
            }
            else {
              $command_count = sizeof($commands) - 1;
              $this->cache = $commands[$command_count];
            }

            for ($index = 0; $index<$command_count; $index++) {
              $command = $commands[$index];
              if (drupal_strlen($command) > 0) {
                $this->handle($command);
              }
            }
          }
        } // end if ($streamstatus['timed_out'])
        if ((time() - $this->info['date']) > $timeout) {
          $this->info['error'] = 'Ping timeout';
        }
      }

      fclose($this->socket);
    }
    else {
      $this->info['error'] = $error_message;
    }

    echo '<span>Disconnected.</span><br />';

    return $this->info;
  }

  protected function handle($command) {
    $command_parts = explode(' ', $command);
    $command_type = $command_parts[0];

    switch ($command_type) {
      case 'BINF':
        $this->info['users'] = $this->info['users'] + 1;
        if (ereg('.* SS([0-9]+).*', $command, $groups)) {
          $this->info['shared'] = $groups[1] + $this->info['shared'];
        }
        if ($command_parts[1] == $this->info['sid']) {
          echo '<span>Checked.</span><br />';
          $this->info['online'] = TRUE;
        }
        break;

      case 'IINF':
        foreach ($command_parts as $named_field) {
          if (drupal_substr($named_field, 0, 2) == 'NI') {
            $this->info['name'] = str_replace('\s', ' ', drupal_substr($named_field, 2));
          }
          elseif (drupal_substr($named_field, 0, 2) == 'DE') {
            $this->info['description'] = str_replace('\s', ' ', drupal_substr($named_field, 2));
          }
          elseif (drupal_substr($named_field, 0, 2) == 'VE') {
            $this->info['hubsoftware'] = str_replace('\s', ' ', drupal_substr($named_field, 2));
          }
        }
        break;

      case 'ISID':
        $this->info['sid'] = $command_parts[1];
        $this->info['pid'] = 'WPBRZB4JF6RA3Y4S7A6S7PXMNTHFK5BEATG56ZQ';
        $this->info['cid'] = 'OTRSQ7QEU2IDJGWHT4Z4OMHV4WYMI7BCWWDNJ2Y';

        $info = 'BINF ' . $this->info['sid'];
        $info .= ' NI' . $this->info['nick'];
        $info .= ' ID' . $this->info['cid'];
        $info .= ' PD' . $this->info['pid'];
        $info .= ' VE' . 'Hublist\\s0.1';
        $info .= ' SS' . $this->info['pingershare'];
        $info .= ' SL' . $this->info['pingerslots'];

        fwrite($this->socket, $info . chr(10));
        break;
    }
  }
}

function base32encode($input) {
  $base32_alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567';

  $output = '';
  $position = 0;
  $storedData = 0;
  $storedBitCount = 0;
  $index = 0;

  while ($index < strlen($input)) {
    $storedData <<= 8;
    $storedData += ord($input[$index]);
    $storedBitCount += 8;
    $index += 1;

    while ($storedBitCount >= 5) {
      $storedBitCount -= 5;
      $output .= $base32_alphabet[$storedData >> $storedBitCount];
      $storedData &= ((1 << $storedBitCount) - 1);
    }
  }

  if ($storedBitCount > 0) {
    $storedData <<= (5 - $storedBitCount);
    $output .= $base32_alphabet[$storedData];
  }

  return $output;
}