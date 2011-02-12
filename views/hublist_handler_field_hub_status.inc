<?php

class hublist_handler_field_hub_status extends views_handler_field_numeric {

  function render($values) {
    $val = $values->{$this->field_alias};
    $ret = t("Unknown");
    switch ($val) {
      case -1:
        $ret = t('Waiting for hublist pinger');
        break;
      case 0:
        $ret = t('Online');
        break;
      case 1:
        $ret = t('Offline');
        break;
      case 2:
        $ret = t('Banned');
        break;
      case 3:
        $ret = t('Inactive');
        break;
      case 4:
        $ret = t('Duplicate');
        break;
    }
    return $ret;
  }
}
