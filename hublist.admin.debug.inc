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

  $form['hublist_list'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ping'),
    '#collapsible' => FALSE,
  );
  $form['hublist_list']['hub_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Hub address'),
    '#default_value' => '',
    '#maxlength' => 255,
    '#description' => t('Specify the full hub URL here including the protocol prefix (dchub://, adc:// or adcs://)'),
    '#required' => TRUE,
  );
  $form['ping'] = array(
    '#type' => 'submit',
    '#value' => t('Ping'),
    '#validate' => array('hublist_admin_debug_validate_url'),
  );

  return $form;
}

function hublist_admin_output(&$form_state) {
  // Pinging hub
  $result = array();
  switch ($form_state['storage']['protocol']) {
    case 'dchub://':
      $pinger = new NmdcPinger();
      $result = $pinger->ping($form_state['storage']['host'], $form_state['storage']['port'], '[HUN]EHPinger_teszt', '', 0, 1, FALSE, 30, TRUE);
      break;
    case 'adc://':
      $pinger = new AdcPinger();
      $result = $pinger->ping($form_state['storage']['host'], $form_state['storage']['port'], '[HUN]EHPinger_teszt', '', 0, 1, FALSE, 30, TRUE);
      break;
    case 'adcs://':
      $pinger = new AdcPinger();
      $result = $pinger->ping($form_state['storage']['host'], $form_state['storage']['port'], '[HUN]EHPinger_teszt', '', 0, 1, TRUE, 30, TRUE);
      break;
  }

  // Output results to the form
  $form = array();

  $form['pinger_output'] = array(
    '#type' => 'fieldset',
    '#title' => t('Output'),
    '#collapsible' => FALSE,
  );
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

// Validating hub url
function hublist_admin_debug_validate_url($form, &$form_state) {
  $address = Util::parseUrl($form_state['values']['hub_url']);
  if ($address['valid']) {
    $form_state['storage']['protocol'] = $address['protocol'];
    $form_state['storage']['host'] = $address['host'];
    $form_state['storage']['port'] = $address['port'];
  }
  else {
    form_set_error('hub_url', t('Please note that the protocol specifier is mandatory for ADC hubs (adc:// or adcs://), optional for NMDC hubs (dchub://), and you need to specify the port denoted by a colon. Examples: adc://test.hub.com:1234, dchub://test.hub.com'));
  }
}

function hublist_admin_debug_clear($form, &$form_state) {
  unset($form_state['storage']);
  $form_state['rebuild'] = TRUE;
}

function hublist_admin_debug_submit($form, &$form_state) {
    $form_state['storage']['result_page'] = TRUE;
    $form_state['rebuild'] = TRUE;
}
