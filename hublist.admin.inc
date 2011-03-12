<?php
/**
 * @file
 * Administration form for Hublist module.
 */
function hublist_admin_settings() {
  $form = array();

  $form['hublist_list'] = array(
    '#type' => 'fieldset',
    '#title' => t('Hublist settings'),
    '#collapsible' => FALSE,
  );
  $form['hublist_list']['hublist_generator_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable hublist generator'),
    '#default_value' => variable_get('hublist_generator_enabled', FALSE),
    '#description' => t('Check this field to enable hublist file generation.'),
  );
  $form['hublist_list']['hublist_generator_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Hublist URL'),
    '#default_value' => variable_get('hublist_generator_url', ''),
    '#maxlength' => 255,
    '#description' => t('The full URL of hublist. This will appear in the XML file.'),
    '#required' => TRUE,
  );
  $form['hublist_list']['hublist_generator_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Hublist name'),
    '#default_value' => variable_get('hublist_generator_name', ''),
    '#maxlength' => 255,
    '#description' => t('Name of hublist. This will appear in the XML file.'),
    '#required' => TRUE,
  );
  $form['hublist_list']['hublist_generator_description'] = array(
    '#type' => 'textfield',
    '#title' => t('Hublist description'),
    '#default_value' => variable_get('hublist_generator_description', ''),
    '#maxlength' => 255,
    '#description' => t('This appears in the \'Description\' column of the XML hublist. Note that the hublist generation time will be added automatically.'),
  );
  $form['hublist_list']['hublist_generator_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Hublist file'),
    '#default_value' => variable_get('hublist_generator_path', ''),
    '#maxlength' => 255,
    '#description' => t('Absolute path of the compressed hublist file. The generated XML list will be saved here.'),
    '#required' => TRUE,
  );
  $form['hublist_list']['hublist_generator_xmlinterval'] = array(
    '#type' => 'textfield',
    '#title' => t('XML file generation interval'),
    '#default_value' => variable_get('hublist_generator_xmlinterval', 0),
    '#maxlength' => 255,
    '#description' => t('Minimum time in minutes between XML file generation (specify 0 for generating it every time cron runs).'),
    '#required' => TRUE,
  );
  $form['hublist_pinger'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pinger settings'),
    '#collapsible' => FALSE,
  );
  $form['hublist_pinger']['hublist_pinger_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable automatic pinger'),
    '#default_value' => variable_get('hublist_pinger_enabled', FALSE),
    '#description' => t('Check this field to enable automatic checking of hubs.'),
  );
  $form['hublist_pinger']['hublist_pinger_nick'] = array(
    '#type' => 'textfield',
    '#title' => t('Pinger nick'),
    '#default_value' => variable_get('hublist_pinger_nick', 'HublistPinger'),
    '#maxlength' => 50,
    '#required' => TRUE,
    '#description' => t('Default nick name for pinger robot.'),
  );
  $form['hublist_pinger']['hublist_pinger_hub_count'] = array(
    '#type' => 'textfield',
    '#title' => t('Checked hubs'),
    '#default_value' => variable_get('hublist_pinger_hub_count', 10),
    '#description' => t('Number of hubs checked at once.'),
  );
  $form['hublist_pinger']['hublist_pinger_maximum_offline_counter'] = array(
    '#type' => 'textfield',
    '#title' => t('Offline threshold count'),
    '#default_value' => variable_get('hublist_pinger_maximum_offline_counter', 100),
    '#description' => t('If a hub is offline this many times in a line, it will become \'Inactive\' and won\'t be pinged automatically anymore.'),
  );
  $form['hublist_advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => FALSE,
  );
  $form['hublist_advanced']['hublist_filter_dupes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Filter duplicated hubs from the list'),
    '#default_value' => variable_get('hublist_filter_dupes', FALSE),
    '#description' => t('This will automatically detect and disable duplicated hubs on pinging.'),
  );
  $form['hublist_advanced']['hublist_pinger_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Output received protocol messages'),
    '#default_value' => variable_get('hublist_pinger_debug', FALSE),
    '#description' => t('Check this field to make NMDC and ADC debug messages appear on your screen when running cron manually. <strong>It is recommended to only enable it on development sites.</strong>'),
  );

  return system_settings_form($form);
}