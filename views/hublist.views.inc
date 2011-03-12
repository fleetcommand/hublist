<?php

// Implementing hook_views_data()
function hublist_views_data() {
  $data = array();
  $data['hublist']['table']['group'] = t('Hublist');
  
  $data['hublist']['table']['join'] = array(
    'node_revisions' => array(
      'left_field' => 'vid',
      'field' => 'vid'
    ),
    'node' => array(
      'left_field' => 'vid',
      'field' => 'vid'
    )
  );
  
  // Hub protocol
  $data['hublist']['hub_protocol'] = array(
    'title' => t('Hub protocol'),
    'help' => t('Protocol of the hub (adc://, adcs:// or dchub://)'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Hub address
  $data['hublist']['hub_address'] = array(
    'title' => t('Hub address'),
    'help' => t('Hub host and port'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Hub URL
  $data['hublist']['hub_url'] = array(
    'title' => t('Hub URL'),
    'help' => t('Full Hub URL so you don\'t have to append the protocol and address together to get a clickable url.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Important hub notes
  $data['hublist']['hub_important'] = array(
    'title' => t('Hub notes'),
    'help' => t('Important notes for the users'),
    'field' => array(
      'handler' => 'views_handler_field_markup',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Website
  $data['hublist']['hub_website'] = array(
    'title' => t('Website'),
    'help' => t('Hub website'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Owner E-mail
  $data['hublist']['hub_owneremail'] = array(
    'title' => t('E-mail'),
    'help' => t('Contact e-mail'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Hub description
  $data['hublist']['hub_description'] = array(
    'title' => t('Description'),
    'help' => t('Description of the hub'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Hub status
  $data['hublist']['hub_status'] = array(
    'title' => t('Hub status'),
    'help' => t('Online status of the hub'),
    'field' => array(
      'handler' => 'hublist_handler_field_hub_status',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'hublist_handler_filter_hub_status',
      'parent' => 'views_handler_filter_in_operator',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Comment for hub status
  $data['hublist']['hub_status_comment'] = array(
    'title' => t('Comment for hub status'),
    'help' => t('Comment for hub status'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Pinger nick
  $data['hublist']['pinger_nick'] = array(
    'title' => t('Pinger nick'),
    'help' => t('Nick of the hublist pinger for the hub if not using the default'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Pinger password
  $data['hublist']['pinger_password'] = array(
    'title' => t('Pinger password'),
    'help' => t('Password for the hublist pinger if required'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Pinger share
  $data['hublist']['pinger_share'] = array(
    'title' => t('Pinger share'),
    'help' => t('Pinger share in GiB'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Latest ping date
  $data['hublist']['stat_pingdate'] = array(
    'title' => t('Latest ping attempt'),
    'help' => t('Latest ping attempt'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );
  
  // Hubsoftware
  $data['hublist']['stat_hubsoftware'] = array(
    'title' => t('Hub software'),
    'help' => t('Hub software'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Usercount
  $data['hublist']['stat_usercount'] = array(
    'title' => t('Usercount'),
    'help' => t('Current user count'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Userlimit
  $data['hublist']['stat_userlimit'] = array(
    'title' => t('Userlimit'),
    'help' => t('Maximum users'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Shared bytes (numeric)
  $data['hublist']['stat_shared'] = array(
    'title' => t('Shared bytes'),
    'help' => t('Currently shared bytes'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Shared bytes (formatted)
  $data['hublist']['stat_shared_formatted'] = array(
    'title' => t('Shared data amount (formatted)'),
    'help' => t('Currently shared bytes (formatted)'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Min share (numeric)
  $data['hublist']['stat_minshare'] = array(
    'title' => t('Minimum share requirement in bytes'),
    'help' => t('Minimum share to enter to the hub in bytes'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Min share (formatted)
  $data['hublist']['stat_minshare_formatted'] = array(
    'title' => t('Minimum share (formatted)'),
    'help' => t('Minimum share to enter to the hub (formatted)'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Minimum slots number
  $data['hublist']['stat_minslots'] = array(
    'title' => t('Minimum slots'),
    'help' => t('Minimum slots you need to open to be able to join'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Maximum hubs
  $data['hublist']['stat_maxhubs'] = array(
    'title' => t('Maximum hubs'),
    'help' => t('Maximum number of hubs'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Successful pings
  $data['hublist']['stat_successful_pings'] = array(
    'title' => t('Successful ping attempts'),
    'help' => t('Successful ping attempts'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // All pings
  $data['hublist']['stat_all_pings'] = array(
    'title' => t('All ping attempts'),
    'help' => t('All ping attempts'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Offline counter
  $data['hublist']['stat_offline_count'] = array(
    'title' => t('Offline counter'),
    'help' => t('Shows how many times is this hub offline in a row'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  // Reliability (formatted)
  $data['hublist']['stat_reliability'] = array(
    'title' => t('Reliability (formatted)'),
    'help' => t('Hub reliability'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  return $data;
} // end function hublist_views_data()

// Implementation of hook_views_handlers().
function hublist_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'hublist') . '/views',
    ),
    'handlers' => array(
      'hublist_handler_filter_hub_status' => array(
        'parent' => 'views_handler_filter',
      ),
      'hublist_handler_field_hub_status' => array(
        'parent' => 'views_handler_field_numeric',
      ),
    ),
  );
} // end function hublist_views_handlers()
