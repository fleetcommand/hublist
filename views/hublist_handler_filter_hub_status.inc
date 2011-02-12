<?php

class hublist_handler_filter_hub_status extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Hub status');
      $options = array(
        '-1' => t('Waiting for hublist pinger'),
        '0' => t('Online'),
        '1' => t('Offline'),
        '2' => t('Banned'),
        '3' => t('Inactive'),
        '4' => t('Duplicate'),
      );
      $this->value_options = $options;
    }
  }
}
