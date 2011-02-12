<?php

class Util {

  // This function adds a http:// prefix for all URL if not beginning with a protocol selector
  public static function fixUrl($url) {
    if (strlen($url) > 0) {
      if (preg_match('/^[a-z]+:\/\//', $url) == 0) {
      $url = 'http://' . $url;
      }
    }
    return $url;
  }

  // This function returns a formatted string for the given amount of bytes
  public static function formatBytes($share) {
    if ($share >= 1024 * 1024 * 1024 * 1024) {
      return round($share / (1024 * 1024 * 1024 * 1024), 2) . ' TiB';
    }
    elseif ($share >= 1024 * 1024 * 1024) {
      return round($share / (1024 * 1024 * 1024), 2) . ' GiB';
    }
    elseif ($share >= 1024 * 1024) {
      return round($share / (1024 * 1024), 2) . ' MiB';
    }
    elseif ($share >= 1024) {
      return round($share / 1024, 2) . ' KiB';
    }
    else {
      return $share . ' B';
    }
  }

  // if the hub url is valid, returns with an array containing the protocol, host and port
  public static function parseUrl($hub_url) {
    $info = array(
      'valid' => FALSE,
      'protocol' => '',
      'host' => '',
      'port' => 0,
    );
    if (preg_match('/^(adcs?:\/\/)([^:]*):([0-9]+)$/', $hub_url, $groups)) {
      // ADC(S) Hub
      $info['valid'] = TRUE;
      $info['protocol'] = $groups[1];
      $info['host'] = $groups[2];
      $info['port'] = $groups[3]; 
    }
    elseif (preg_match('/^(dchub:\/\/)?([^:]*)(:([0-9]+))?$/', $hub_url, $groups)) {
      // NMDC Hub
      $info['valid'] = TRUE;
      $info['protocol'] = 'dchub://';
      $info['host'] = $groups[2];
      if(isset($groups[4]))
        $info['port'] = $groups[4];
      else
        $info['port'] = 411;
    }
    return $info;
  }

} // end class Util