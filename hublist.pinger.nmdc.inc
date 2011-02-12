<?php

class NmdcPinger {
  private $cache, $complete, $info, $socket;
  private $socket_error_msg = "OK", $socket_error_no = 0;

  public function ping($host, $port, $nick, $password, $share = '0', $slots = '1', $secure = FALSE, $timeout = 30, $debug = FALSE) {

    set_error_handler('self::handleError');

    $this->disconnect = FALSE;

    if ($port == 0) {
      $port = 411;
    }

    $this->info = array(
      'address' => gethostbyname($host),
      'country' => '',
      'date' => REQUEST_TIME,
      'description' => '',
      'error' => '',
      'host' => $host,
      'name' => '',
      'nick' => $nick,
      'password' => $password,
      'pingershare' => $share,
      'pingerslots' => $slots,
      'port' => $port,
      'protocol' => 'dchub://',
      'shared' => 0,
      'online' => FALSE,
      'pingerextension' => FALSE,
      'nicklist' => array(),
      'users' => 0,
      'warning' => '',
      'userlimit' => 0,
      'minshare' => 0,
      'minslots' => 0,
      'maxhubs' => 0,
      'hubsoftware' => '',
      'debug' => '',
    );

    if ($debug)
      $this->debug('Connecting to ' . $this->info['protocol'] . $this->info['host'] . ':' . $this->info['port'] . '...');

    try {
      $this->socket = fsockopen(($secure ? 'ssl://' : '') . $this->info['host'], $this->info['port'], $socket_error_no, $socket_error_message, $timeout);
    }

    catch (Exception $e) {
      if ($socket_error_no != 0) {
        $this->info['error'] = $socket_error_message;
      }
      else {
        $this->info['error'] = $e->getMessage();
      }
    }

    if ($this->socket) {
      stream_set_timeout($this->socket, 5);
      while (!feof($this->socket) && !$this->disconnect) {
        $this->cache .= fread($this->socket, 2048);

        $streamstatus = stream_get_meta_data($this->socket);

        if ($streamstatus['timed_out']) {
          $this->info['warning'] = 'Timeout on reading';
          if ($debug)
            $this->debug('@@ TIMEOUT');

          if (!$this->info['online']) {
            // If we timeout even before receiving a $Hello, that's a bad sign. Disconnect.
            $this->info['error'] = 'Timeout during handshake';
            $this->disconnect = TRUE;
          }
        }
        else {
          if ($this->cache != '') {
            $commands = explode('|', $this->cache);

            if ($this->cache[strlen($this->cache) -1] == '|') {
              $command_count = sizeof($commands);
              $this->cache = '';
            }
            else {
              $command_count = sizeof($commands) - 1;
              $this->cache = $commands[$command_count];
            } // end if

            for ($index = 0; $index < $command_count; $index++) {
              if ($this->disconnect) {
              break;
              }
              $command = $commands[$index];
              if (strlen($command) > 0) {
                $this->handle($command, $debug);
              } // end if
            } // end for
          } // end if($this->cache != '')
        } // end if($streamstatus['timed_out'])

        if ((REQUEST_TIME - $this->info['date']) > $timeout) {
          if (!$this->info['online']) {
            $this->info['error'] = 'Ping timeout';
          }
          else {
            // TODO: REMOVE THIS since this interferes with the No pinger whatever
            $this->info['error'] = 'Maximum time on hub exceeded';
          }
          $this->disconnect = TRUE;
        } // end if
      } // end while
    fclose($this->socket);
    }

    if ($debug)
      $this->debug('Disconnected.');

    restore_error_handler();
    return $this->info;
  } // end method ping

  private function handle($command, $debug) {

    if ($debug)
      $this->debug('<< ' . $command);

    $command_parts = explode(' ', $command);
    $command_type = $command_parts[0];

    switch ($command_type) {
      case '$BadPass':
        $this->info['error'] = 'Incorrect password';
        $this->disconnect = TRUE;
        break;
      case '$GetPass':
        $this->send('$MyPass ' . $this->info['password'], $debug);
        break;
      case '$Lock':
        $this->send('$Supports NoHello NoGetINFO BotINFO', $debug);
        $this->send('$Key ' . $this->lock2key($command_parts[1]), $debug);
        $this->send('$ValidateNick ' . $this->info['nick'], $debug);
        break;
      case '$Hello':
        if ($command_parts[1] == $this->info['nick']) {
        $this->info['online'] = TRUE;
        $this->send('$Version 1.0091', $debug);
        $this->send('$MyINFO $ALL ' . $this->info['nick'] . ' <Hublist V:0.1,M:P,H:1/0/0,S:' . $this->info['pingerslots'] . '>$ $1' . chr(1) . '$$' . $this->info['pingershare'] . '$', $debug);
        $this->send('$GetNickList', $debug);
        $this->send('$BotINFO Hublist', $debug);
        }
        break;
      case '$HubINFO':
        // $HubINFO <name>$<host:port>$<description>$<max users>$<min share>$<min slots>$<max hubs>$<hubtype>$<owner>|
        $hubinfo = explode('$', substr($command, 9));
        try {
          $this->info['name'] = $this->unescape_nmdc(iconv('windows-1250', 'UTF-8', $hubinfo[0]));
          $this->info['description'] = iconv('windows-1250', 'UTF-8', $hubinfo[2]);
          $this->info['userlimit'] = iconv('windows-1250', 'UTF-8', $hubinfo[3]);
          $this->info['minshare'] = iconv('windows-1250', 'UTF-8', $hubinfo[4]);
          $this->info['minslots'] = iconv('windows-1250', 'UTF-8', $hubinfo[5]);
          $this->info['maxhubs'] = iconv('windows-1250', 'UTF-8', $hubinfo[6]);
          $this->info['hubsoftware'] = iconv('windows-1250', 'UTF-8', $hubinfo[7]);
        }
        catch (Exception $e) {
          $this->info['warning'] = "Couldn't process HubINFO because of unexpected characters.";
        }
        // This is a workaround for Verlibug, since that stupid hub doesn't send HubInfo in its support
        // However it sends the HubINFO command itself.
        $this->info['pingerextension'] = TRUE;

        $this->disconnect = TRUE;
        break;
      case '$HubName':
        // Hopefully we will get a proper hubname with $HubINFO later.
        // Note that we don't need to verify if we already got one since $HubINFO causes
        // automatic disconnection.
        try {
          $this->info['name'] = $this->unescape_nmdc(iconv('windows-1250', 'UTF-8', substr($command, 9)));
        }
        catch (Exception $e) {
          $this->info['warning'] = "Couldn't process HubName because of unexpected characters.";
        }
        break;
      case '$HubIsFull':
        $this->info['error'] = 'Hub is full';
        $this->disconnect = TRUE;
        break;
      case '$MyINFO':
        // $MyINFO $ALL <nick> <description>$ $<connection><flag>$<e-mail>$<share>$
        $myinfo = explode(' ', substr($command, 13));
        try {
          $nick = iconv('windows-1250', 'UTF-8', $myinfo[0]);
          if (!array_key_exists($nick, $this->info['nicklist'])) {
            $this->info['nicklist'][$nick] = array('shared' => 0);
            $this->info['users'] = $this->info['users'] + 1;
            if (preg_match('/^.*\$([0-9]+)\$$/', $command, $groups)) {
              $this->info['shared'] += $groups[1];
              $this->info['nicklist'][$nick]['shared'] = $groups[1];
            }
          } // end if
        }
        catch (Exception $e) {
          $this->info['warning'] = "Couldn't process all users because of unexpected characters in nick.";
        }
        break;
      case '$Quit':
        try {
          $nick = iconv('windows-1250', 'UTF-8', $command_parts[1]);
          if (array_key_exists($nick, $this->info['nicklist'])) {
            $this->info['users'] = $this->info['users'] - 1;
            $this->info['shared'] -= $this->info['nicklist'][$nick]['shared'];
            unset($this->info['nicklist'][$nick]);
          }
        }
        catch (Exception $e) {
          $this->info['warning'] = "Couldn't process all users because of unexpected characters in nick.";
        }
        break;
      case '$Supports':
        if (strpos($command, 'HubINFO') || strpos($command, 'BotINFO')) {
          // Please note that some stupid Verlihubs supports HubINFO but doesn't advertise it
          // However, hub owners can fix it, but in that case the hub sends two separate $Supports.
          // This is why we have to look for the existense of HubINFO and not the non-existense
          $this->info['pingerextension'] = TRUE;
        }
        break;
      case '$ValidateDenide':
        $this->info['error'] = 'Incorrect nick';
        $this->disconnect = TRUE;
        break;
    } // end switch
  } // end method handle

  private function send($command, $debug) {
    if ($debug)
      $this->debug('>> ' . $command);

    fwrite($this->socket, $command . '|');
  }

  private function debug($msg) {
    if ($this->info['debug'] != '')
      $this->info['debug'] .= "\n";
    try {
      $this->info['debug'] .= iconv('windows-1250', 'UTF-8', $msg);
    }
    catch (Exception $e) {
      $this->info['debug'] .= '## Warning: Could not convert characters to UTF-8';
    }
  }

  private function lock2key($lock) {
    $key = array();

    $length = strlen($lock);
    $key[0] = ord($lock[0]) ^ ord($lock[$length -1]) ^ ord($lock[$length -2]) ^ 5;

    for ($i = 1; $i < $length; $i++) {
      $key[$i] = ord($lock[$i]) ^ ord($lock[$i -1]);
    }
    for ($i = 0; $i < $length; $i++) {
      $key[$i] = (($key[$i] << 4) & 240) | (($key[$i] >> 4) & 15);
    }

    $key = array_map('chr', $key);

    for ($i = 0; $i < $length; $i++) {
      if ($key[$i] == chr(0)) {
        $key[$i] = '/%DCN000%/';
      }
      elseif ($key[$i] == chr(5)) {
        $key[$i] = '/%DCN005%/';
      }
      elseif ($key[$i] == chr(36)) {
        $key[$i] = '/%DCN036%/';
      }
      elseif ($key[$i] == chr(96)) {
        $key[$i] = '/%DCN096%/';
      }
      elseif ($key[$i] == chr(124)) {
        $key[$i] = '/%DCN124%/';
      }
      elseif ($key[$i] == chr(126)) {
        $key[$i] = '/%DCN126%/';
      }
    }
    return implode('', $key);
  } // end method lock2key

  private function unescape_nmdc($text) {
    $escaped = array('&#36;', '&#124;');
    $unescaped = array('$', '|');
    return str_replace($escaped, $unescaped, $text);
  }

  // This function converts php errors to exceptions
  // Used by :run() to catch pinger exceptions
  private static function handleError($errno, $errstr, $errfile, $errline, array $errcontext) {
    // error was suppressed with the @-operator
    if (0 === error_reporting()) {
      return FALSE;
    }
    throw new ErrorException($errstr, 0, $errno, $errfile, $errline);
  }
}