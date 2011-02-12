<?php
// $Id$

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hublist_admin_debug($form, &$form_state) {
  if (isset($form_state['storage']['result_page'])) {
    return hublist_admin_output(&$form_state);
  }
  $form = array();

  $form['group_ping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ping hub'),
    '#collapsible' => FALSE,
  );
  $form['group_ping']['hub_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Hub address'),
    '#default_value' => '',
    '#maxlength' => 255,
    '#description' => t('Specify the full hub URL here. For an ADC hub, using the protocol specifier (adc:// or adcs://) and the port are mandatory, for NMDC hub using the dchub:// is optional, and you only need to specify the port if the hub uses other than the default (411).'),
    '#required' => TRUE,
  );
  $form['group_advanced'] = array(
    '#type' => 'fieldset',
	'#title' => t('Optional settings'),
	'#collapsible' => TRUE,
	'#collapsed' => TRUE,
  );
  $form['group_advanced']['pinger_nick'] = array(
    '#type' => 'textfield',
    '#title' => t('Pinger nick'),
    '#default_value' => variable_get('hublist_pinger_nick', 'HublistPinger'),
    '#maxlength' => 50,
    '#required' => TRUE,
    '#description' => t('The user name of the pinger used on this hub.'),
  );
  $form['group_advanced']['pinger_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Pinger password'),
    '#description' => t('Enter the password here if required for connecting.'),
    '#default_value' => '',
    '#maxlength' => 32,
  );
  $form['group_advanced']['pinger_slots'] = array(
    '#type' => 'textfield',
    '#title' => t('Opened slots claimed by the pinger'),
    '#description' => t('Specify here how many open slots should the pinger claim to have when checking this hub.'),
    '#default_value' => 1,
    '#maxlength' => 2,
    '#required' => TRUE,
  );
  $form['group_advanced']['pinger_share'] = array(
    '#type' => 'textfield',
    '#title' => t('Pinger share (GiB)'),
    '#description' => t('Specify the share size the pinger should use to ping this hub.'),
    '#default_value' => 0,
    '#maxlength' => 5,
    '#required' => TRUE,
  );
  $form['ping'] = array(
    '#type' => 'submit',
    '#value' => t('Ping'),
    '#validate' => array('hublist_admin_debug_validate_pinger_settings'),
  );

  return $form;
}

function hublist_admin_output(&$form_state) {
  // Pinging hub
  $result = array();
  switch ($form_state['storage']['protocol']) {
    case 'dchub://':
      $pinger = new NmdcPinger();
      $result = $pinger->ping($form_state['storage']['host'], $form_state['storage']['port'], $form_state['storage']['pinger_nick'], $form_state['storage']['pinger_password'], $form_state['storage']['pinger_share'] * 1073741824 + 1, 1, FALSE, 30, TRUE);
      break;
    case 'adc://':
      $pinger = new AdcPinger();
      $result = $pinger->ping($form_state['storage']['host'], $form_state['storage']['port'], $form_state['storage']['pinger_nick'], $form_state['storage']['pinger_password'], $form_state['storage']['pinger_share'] * 1073741824 + 1, 1, FALSE, 30, TRUE);
      break;
    case 'adcs://':
      $pinger = new AdcPinger();
      $result = $pinger->ping($form_state['storage']['host'], $form_state['storage']['port'], $form_state['storage']['pinger_nick'], $form_state['storage']['pinger_password'], $form_state['storage']['pinger_share'] * 1073741824 + 1, 1, TRUE, 30, TRUE);
      break;
  }

  // Output results to the form
  $form = array();

  $form['pinger_output'] = array(
    '#type' => 'fieldset',
    '#title' => t('Output'),
    '#collapsible' => FALSE,
  );
  // TODO: Add a reminder here with the actual settings used for ping
  // Such as: $form_state['storage']['pinger_nick'], $form_state['storage']['pinger_password'], $form_state['storage']['pinger_share']
  $form['pinger_output']['result'] = array(
    '#type' => 'textarea',
    '#title' => t('Result'),
    '#rows' => 20,
    '#default_value' => $result['debug'],
    '#description' => t('You can see the protocol messages here. Experienced admins probably understand their meaning.'),
  );
  $form['ok'] = array(
    '#type' => 'submit',
    '#value' => t('Done'),
    '#validate' => array('hublist_admin_debug_clear'),
  );

  return $form;
}

// Validating Pinger settings
function hublist_admin_debug_validate_pinger_settings($form, &$form_state) {
  // Verifying hub address
  $address = Util::parseUrl($form_state['values']['hub_url']);
  if ($address['valid']) {
    $form_state['storage']['protocol'] = $address['protocol'];
    $form_state['storage']['host'] = $address['host'];
    $form_state['storage']['port'] = $address['port'];
  }
  else {
    form_set_error('hub_url', t('Please note that the protocol specifier is mandatory for ADC hubs (adc:// or adcs://), optional for NMDC hubs (dchub://), and you need to specify the port denoted by a colon. Examples: adc://test.hub.com:1234, dchub://test.hub.com'));
  }
  // Spaces in nick
  if (strpos($form_state['values']['pinger_nick'], " ")) {
    form_set_error("pinger_nick", t("<strong>Pinger nick</strong> cannot contain spaces."));
  }
  // Is pinger share a whole number?
  if (preg_match("/^[0-9]{1,5}$/", $form_state['values']['pinger_share']) == 0) {
    form_set_error("pinger_share", t("<strong>Pinger share</strong> must be a whole number with a maximum length of 5 characters."));
  }
  // Is pinger slots a number with a reasonable value?
  if (preg_match("/^[0-9]{1,2}$/", $form_state['values']['pinger_slots']) == 0) {
    form_set_error("pinger_slots", t("<strong>Slot number</strong> for the pinger must be a whole number between 0 and 99."));
  }
}

function hublist_admin_debug_clear($form, &$form_state) {
  unset($form_state['storage']);
  $form_state['rebuild'] = TRUE;
}

function hublist_admin_debug_submit($form, &$form_state) {
    $form_state['storage']['result_page'] = TRUE;
	// Forwarding values to the second form. The hub address is already set,
	// now we put the rest into the array
	$form_state['storage']['pinger_nick'] = $form_state['values']['pinger_nick'];
	$form_state['storage']['pinger_share'] = $form_state['values']['pinger_share'];
	$form_state['storage']['pinger_slots'] = $form_state['values']['pinger_slots'];
	$form_state['storage']['pinger_password'] = $form_state['values']['pinger_password'];
    $form_state['rebuild'] = TRUE;
}
