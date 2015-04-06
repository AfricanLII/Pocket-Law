<?php
// $Id: sphinx_admin.inc.php,v 1.2 2008/09/25 13:13:19 johsw Exp $

function _check_searchd($host = SPH_DEFAULT_HOST, $port = SPH_DEFAULT_PORT) {
  $cl = new SphinxClient();
  $cl->SetServer($host, $port);
  $connect = $cl->_Connect();
  if (!$connect) {
    drupal_set_message(t('Searchd not running'), 'warning');
  }
}

/**
 * The admin page callback
 */
function _sphinx_admin() {
  _check_searchd();
  return drupal_get_form('sphinx_admin_form');
}

function _sphinx_admin_indexes($op = '', $iid = '') {
  _check_searchd();
  if (!empty($op) && !empty($iid)) {
    switch ($op) {
      case 'delete':
        return drupal_get_form('sphinx_index_confirm_delete', $iid);
      break;
      
      case 'edit':
        return drupal_get_form('sphinx_index_edit', $iid);
      break;
      
      case 'enable':
        db_query('UPDATE {sphinx_indexes} SET active = 1 WHERE iid =%d', $iid);
        drupal_goto('admin/settings/sphinx/indexes');
        break;
      
      case 'disable':
        db_query('UPDATE {sphinx_indexes} SET active = 0 WHERE iid =%d', $iid);
        drupal_goto('admin/settings/sphinx/indexes');
        break;
      
      case 'fields':
        return _sphinx_manage_fields($iid);
      break;
      
      case 'attributes':
        return _sphinx_manage_attributes($iid);
      break;
    }
  }
 
  $header = array(
    array('data' => t('Index'), 'field' => 'index_name'),
    array('data' => t('Display name'), 'field' => 'display_name'),
    array('data' => t('Path'), 'field' => 'path'),
    array('data' => t('Sort'), 'field' => 'sort_field'),
    array('data' => t('Order'), 'field' => 'default_sort_order'),
	array('data' => t('MatchPath'), 'field' => 'match_path'),
    array('data' => t('Server'), 'field' => 'server'),
    array('data' => t('Port'), 'field' => 'port'),
    array('data' => t('Excerpt'), 'field' => 'excerpt'),
    array('data' => t('Multi-query'), 'field' => 'multiquery'),
    array('data' => t('Active'), 'field' => 'active'),
    array('data' => t('')),
    array('data' => t('')),
    array('data' => t('')),
    array('data' => t('')),
    array('data' => t('')),
  );
  
  $sql     = 'SELECT {sphinx_indexes}.*, {sphinx_attributes}.display_name AS sort_field FROM {sphinx_indexes} LEFT JOIN {sphinx_attributes} ON default_sort_key_fid = aid '. tablesort_sql($header);
  $result  = pager_query($sql, 10);
  
  
  
  while ($indexes = db_fetch_object($result)) {
    if ($indexes->active) {
	  $client  = new SphinxClient();
   	  $client->SetServer($indexes->server, (int) $indexes->port);
      $client->SetLimits(0, 1);
	  $client->SetConnectTimeout(60);
	  $client->SetWeights ( array ( 100, 1 ) );
	  $client->SetArrayResult ( true );
	  $client->SetMatchMode ( SPH_MATCH_ALL );
	  // original source code queried on an empty string.  this appears to work from the CLI (that is, it gets a record count), but not from the 
	  // PHP API as it now stands.  There must be a better way to query for index status.
      $res = $client->Query('*e*', $indexes->index_name);
      if ($res) {
        $status = 'Serving ('. $res['total_found'] .')';
      }
      else {
		dpm($client -> GetLastError());
		dpm($client);
        $status = '<span class="error">Error</span>';
      }
    }
    else {
      $status = 'Disabled';
      $res = false;
    }
    $active = ($indexes->active == 0) ? l(t('Enable'), 'admin/settings/sphinx/indexes/enable/'. $indexes->iid) : l(t('Disable'), 'admin/settings/sphinx/indexes/disable/'. $indexes->iid);
    $rows[] = array(
      $indexes->index_name,
      $indexes->display_name,
      $indexes->path,
      $indexes->sort_field,
      $indexes->default_sort_order,
	  $indexes->match_path,
      $indexes->server,
      $indexes->port,
      $indexes->excerpt ? t('Enabled') : t('Disabled'),
      $indexes->multiquery ? t('Enabled') : t('Disabled'),
      $status,
      ($res) ? l(t('Fields'), 'admin/settings/sphinx/indexes/fields/'. $indexes->iid) : '',
      ($res) ? l(t('Attributes'), 'admin/settings/sphinx/indexes/attributes/'. $indexes->iid) : '',
      l(t('Delete'), 'admin/settings/sphinx/indexes/delete/'. $indexes->iid),
      $active,
      l(t('Edit'), 'admin/settings/sphinx/indexes/edit/'. $indexes->iid),
    );
	unset ($client);
  }
  $output = '';
  $output .= theme('table', $header, $rows);
  $output .= drupal_get_form('sphinx_admin_add_index_form');
  $output .= theme_pager(null, 10, 10);
  #$output .= theme('pager', null, 10, 10);
  
  return $output;
}

/**
 * FAPI definition for the sphinx administration form.
 *
 * ...
 * @ingroup forms
 * @see sphinx_admin_form_submit()
 */
function sphinx_admin_form() {
  
  $options = array();
  for ($n = 1; $n < 10; $n++) {
    $options[$n]       = $n;
    $options[$n * 10]  = $n * 10;
    $options[$n * 100] = $n * 100;
  }
  asort($options, SORT_NUMERIC);
  $sql           = 'SELECT iid, index_name, display_name FROM {sphinx_indexes}';
  $result        = db_query($sql);
  $index_options = array();
  while ($indexes = db_fetch_object($result)) {
    $index_options[$indexes->iid] = $indexes->display_name .' ('. $indexes->index_name .')';
  }
  
  $form['sphinx']['sphinx_offline_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Offline message'),
    '#default_value' => variable_get('sphinx_offline_message', 'The search engine is unfortunately not working at the moment. Please return later'),
    '#description' => t('Type the message you users will see, if the search daemon is not running'),
    '#required' => true,
  );
  
  $form['sphinx']['sphinx_default_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Default server'),
    '#default_value' => variable_get('sphinx_default_server', 'localhost'),
    '#description' => t('Type the hostname of the server where you main searchd is running (this setting can be altered per index)'),
  );
  $form['sphinx']['sphinx_default_port'] = array(
    '#type' => 'textfield',
    '#title' => t('Default port'),
    '#default_value' => variable_get('sphinx_default_port	', '3312'),
    '#description' => t('Type the port on which your default searchd is listening (this setting can be altered per index)'),
  );
  $form['sphinx']['sphinx_default_index'] = array(
    '#type' => 'select',
    '#title' => t('Default index'),
    '#default_value' => variable_get('sphinx_default_index', '1'),
    '#options' => $index_options,
    '#description' => t('Select the default index'),
  );
  $form['sphinx']['sphinx_results_per_page'] = array(
    '#type' => 'select',
    '#title' => t('Results pr. page'),
    '#default_value' => variable_get('sphinx_results_per_page', 10),
    '#options' => $options,
    '#description' => t('Select the default number of search results displayed to the user'),
  );
  return system_settings_form($form);
}

//function sphinx_admin_form_validate($form_id, &$form_state) {
  //TODO insert check for portnum and server ip
//}

function sphinx_admin_form_submit($form_id, $form_state) {
  variable_set('sphinx_offline_message', $form_state['values']['sphinx_offline_message']);
  variable_set('sphinx_default_server', $form_state['values']['sphinx_default_server']);
  variable_set('sphinx_default_port', $form_state['values']['sphinx_default_port']);
  variable_set('sphinx_default_index', $form_state['values']['sphinx_default_index']);
  variable_set('sphinx_results_per_page', $form_state['values']['sphinx_results_per_page']);
  drupal_set_message(t('Your Sphinx settings are saved'));
}

function sphinx_admin_add_index_form() {
  $form['sphinx']['addnew'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add new index'),
    '#weight' => 5,
    '#collapsible' => true,
    '#collapsed' => true,
  );
  
  $form['sphinx']['addnew']['index_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Index name'),
    '#description' => t('Type the name of the index you want you users to search'),
    '#required' => true,
  );
  $form['sphinx']['addnew']['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display name'),
    '#description' => t('Type the name you want the users to see'),
    '#required' => true,
  );
  $form['sphinx']['addnew']['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#description' => t('Type the path of this index. If you type <i>products</i>, the path will be <i>/find/products</i>'),
    '#required' => true,
  );
  $form['sphinx']['addnew']['match_path'] = array(
    '#type' => 'textfield',
    '#title' => t('MatchPath'),
    '#description' => t('If this index applies only to part of your site, enter a pattern that will match URLs where it should be used'),
    '#required' => false,
  );
  $form['sphinx']['addnew']['server'] = array(
    '#type' => 'textfield',
    '#title' => t('Server'),
    '#description' => t('Type the name of the server on which the searchd of this index is running'),
    '#required' => true,
    '#default_value' => variable_get('sphinx_default_server', 'localhost'),
  );
  $form['sphinx']['addnew']['port'] = array(
    '#type' => 'textfield',
    '#title' => t('Port'),
    '#description' => t('Type the name the number of the port the searchd for this index is listening'),
    '#required' => true,
    '#default_value' => variable_get('sphinx_default_port', '3312'),
  );
  $form['sphinx']['addnew']['excerpt'] = array(
    '#type' => 'checkbox',
    '#title' => t('Excerpt'),
    '#description' => t('Do you want to display excerpts for results from this index?'),
  );
  $form['sphinx']['addnew']['multiquery'] = array(
    '#type' => 'checkbox',
    '#title' => t('Multiquery'),
    '#description' => t('Does this index provide multiquerys for facets or the like?'),
  );
  $form['sphinx']['addnew']['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Should this index be activated?'),
  );
  $form['sphinx']['addnew']['submit'] = array('#type' => 'submit', '#value' => t('Submit'));
  return $form;
}

/* function sphinx_admin_add_index_form_submit($form_id, $form_values) {
  db_query("INSERT INTO {sphinx_indexes} (index_name, display_name, path, server, port, excerpt, multiquery, active)  VALUES ('%s','%s','%s',%d,%d,%d,%d,%d)", $form_values['index_name'], $form_values['display_name'], $form_values['path'], $form_values['server'], $form_values['port'], $form_values['excerpt'], $form_values['multiquery'], $form_values['active']);
  } */
function sphinx_admin_add_index_form_submit($form_id, $form_state) {
  db_query("INSERT INTO {sphinx_indexes} (index_name, display_name, path,match_path, server, port, excerpt, multiquery, active)  VALUES ('%s','%s','%s','%s','%s',%d,%d,%d,%d)", $form_state['values']['index_name'], $form_state['values']['display_name'], $form_state['values']['path'], $form_state['values']['match_path'],$form_state['values']['server'], $form_state['values']['port'], $form_state['values']['excerpt'], $form_state['values']['multiquery'], $form_state['values']['active']);
}

function sphinx_index_confirm_delete($form_state, $iid = '') {
  
  
  if (!empty($iid)) {
    $result       = db_query('SELECT index_name, display_name FROM {sphinx_indexes} WHERE iid = %d;', $iid);
    $names        = db_fetch_object($result);
    $form         = array();
    $form['text'] = array(
      '#value' => t('Are you sure you want to delete the index <strong>%title</strong> (<i>%display</i>)? ', array('%title' => $names->index_name, '%display' => $names->display_name)),
    );
    $form['index']  = array('#type' => 'hidden', '#value' => $iid);
    $form['submit'] = array('#type' => 'submit', '#value' => t('Delete'));
    $form['cancel'] = array('#type' => 'submit', '#value' => t('Cancel'));
    
    return $form;
  }
}

function sphinx_index_confirm_delete_submit($form_id, $form_state) {
  if ($form_state['values']['op'] == 'Delete') {
    db_query('DELETE FROM {sphinx_indexes} WHERE iid = %d;', $form_state['values']['index']);
    db_query('DELETE FROM {sphinx_fields} WHERE iid = %d;', $form_state['values']['index']);
    db_query('DELETE FROM {sphinx_attributes} WHERE iid = %d;', $form_state['values']['index']);
  }
  drupal_goto('admin/settings/sphinx/indexes');
}

function sphinx_index_edit(&$form_state = NULL, $iid = '') { 

  if (!empty($iid)) {
    $result  = db_query('SELECT * FROM {sphinx_indexes} WHERE iid = %d;', $iid);
    $index   = db_fetch_object($result);
    
    $sql     = 'SELECT aid, attribute_name, display_name FROM {sphinx_attributes} WHERE iid=%d AND active=1';
    $result  = db_query($sql, $iid);

    $options = array();
    while ($attributes = db_fetch_object($result)) {
      $options[$attributes->aid] = $attributes->display_name;
    }
	
    $form['sphinx']['index_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Index name'),
      '#description' => t('Type the name of the index you want you users to search'),
      '#required' => true,
      '#default_value' => $index->index_name,
    );
    $form['sphinx']['display_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Display name'),
      '#description' => t('Type the name you want the users to see'),
      '#required' => true,
      '#default_value' => $index->display_name,
    );
    $form['sphinx']['path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path'),
      '#description' => t('Type the path of this index. If you type <i>products</i>, the path will be <i>/find/products</i>'),
      '#required' => true,
      '#default_value' => $index->path,
    );
	$form['sphinx']['match_path'] = array(
      '#type' => 'textfield',
      '#title' => t('MatchPath'),
      '#description' => t('URL pattern to match. This causes the index to be invoked for particular parts of your site.'),
      '#required' => false,
      '#default_value' => $index->match_path,
    );
    $form['sphinx']['default_sort_key_fid'] = array(
      '#type' => 'select',
      '#title' => t('Default sort'),
      '#options' => $options,
      '#default_value' => $index->default_sort_key_fid,
      '#description' => t('Select the field to use for ordering results'),
    );
    $form['sphinx']['default_sort_order'] = array(
      '#type' => 'select',
      '#title' => t('Default order'),
      '#options' => array('asc' => t('Ascending'), 'desc' => t('Descending')),
      '#default_value' => $index->default_sort_order,
      '#description' => t('Select which direction results should be ordered by default'),
    );
    $form['sphinx']['server'] = array(
      '#type' => 'textfield',
      '#title' => t('Server'),
      '#description' => t('Type the name the name of the server on which the searchd of this index is running'),
      '#required' => true,
      '#default_value' => $index->server,
    );
    $form['sphinx']['port'] = array(
      '#type' => 'textfield',
      '#title' => t('Port'),
      '#description' => t('Type the name the number of the port the searchd for this index is listening'),
      '#required' => true,
      '#default_value' => $index->port,
    );
    $form['sphinx']['excerpt'] = array(
      '#type' => 'checkbox',
      '#title' => t('Excerpt'),
      '#description' => t('Do you want to display excerpts for results from this index?'),
      '#default_value' => $index->excerpt,
    );
    $form['sphinx']['multiquery'] = array(
      '#type' => 'checkbox',
      '#title' => t('Multiquery'),
      '#description' => t('Does this index provide multiquerys for facets or the like?'),
      '#default_value' => $index->multiquery,
    );
    $form['sphinx']['active'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable'),
      '#description' => t('Should this index be activated?'),
      '#default_value' => $index->active,
    );
    $form['sphinx']['index'] = array('#type' => 'textfield', '#title' => t('iid'), '#default_value' => $iid);
    $form['sphinx']['submit'] = array('#type' => 'submit', '#value' => t('Submit'));
    return $form;
  }
}

function sphinx_index_edit_submit($form_id, $form_state) {
  db_query("UPDATE {sphinx_indexes} SET index_name='%s', display_name='%s', path='%s', match_path='%s',default_sort_key_fid='%s', default_sort_order='%s', server='%s', port=%d, excerpt=%d, multiquery=%d, active=%d WHERE iid=%d;",
    $form_state['values']['index_name'],
    $form_state['values']['display_name'],
    $form_state['values']['path'],
	$form_state['values']['match_path'],
    $form_state['values']['default_sort_key_fid'],
    $form_state['values']['default_sort_order'],
    $form_state['values']['server'],
    $form_state['values']['port'],
    $form_state['values']['excerpt'],
    $form_state['values']['multiquery'],
    $form_state['values']['active'],
    $form_state['values']['index']
    );
  drupal_goto('admin/settings/sphinx/indexes');
}

function _sphinx_manage_fields($iid) {
  
  $output = '';
  $sql    = 'SELECT * FROM {sphinx_indexes} WHERE iid=%d;';
  $result = db_query($sql, $iid);
  $index  = db_fetch_object($result);
  $client = new SphinxClient();
  $client->SetServer($index->server, (int)$index->port);
  $connect = $client->_Connect();
  if (!$connect) {
    return ('Index not available to manage_fields: '.'iid='.$iid.' '. $index->server . ':' . $index->port);
  }
  $client->SetLimits(0, 1);
  $res     = $client->Query('*e*', $index->index_name);
  $serving = $res['fields'];
  $header  = array(
    array('data' => t('Field'), 'field' => 'field_name'),
    array('data' => t('Display name'), 'field' => 'display_name'),
    array('data' => t('Excerpt'), 'field' => 'excerpt'),
    array('data' => t('Weight'), 'field' => 'weight', 'sort' => 'desc'),
    array('data' => t('Active'), 'field' => 'active'),
    array('data' => t('Served')),
    array(),
    array(),
  );
  
  $sql     = 'SELECT * FROM {sphinx_fields} WHERE iid=%d '. tablesort_sql($header);
  $result  = db_query($sql, $iid);
  $counter = 0;
  while ($fields = db_fetch_object($result)) {
    $counter++;
    if (in_array($fields->field_name, $serving)) {
      unset($serving[array_search($fields->field_name, $serving)]);
      $status = 'Serving';
    }
    else {
      $status = '<span class="error">Not served!</span>';
    }
    $active = ($fields->active == 0) ? l(t('Enable'), 'admin/settings/sphinx/fields/enable/'. $iid .'/'. $fields->fid) : l(t('Disable'), 'admin/settings/sphinx/fields/disable/'. $iid .'/'. $fields->fid);
    $rows[] = array(
      $fields->field_name,
      $fields->display_name,
      $fields->excerpt ? 'Enabled' : 'Disabled',
      $fields->weight,
      $fields->active ? 'Enabled' : 'Disabled',
      $status,
      l(t('Delete'), 'admin/settings/sphinx/fields/delete/'. $iid .'/'. $fields->fid),
      $active,
      l(t('Edit'), 'admin/settings/sphinx/fields/edit/'. $iid .'/'. $fields->fid),
    );
  }
  if ($counter) {
    $output .= theme('table', $header, $rows);
  }
  if (count($serving)) {
    
    $add_header = array(
      array('data' => t('Field')),
      array(),
    );
    foreach ($serving as $field) {
      $add_rows[] = array($field, l(t('Add'), 'admin/settings/sphinx/fields/add/'. $iid .'/'. $field));
    }
    $output .= theme('table', $add_header, $add_rows);
  }
  return $output;
}

function _sphinx_alter_fields($op, $iid, $field) {
  switch ($op) {
    case 'delete':
      return drupal_get_form('sphinx_field_confirm_delete', $iid, $field);
    break;
    
    case 'edit':
      return drupal_get_form('sphinx_field_edit', $iid, $field);
    break;
    
    case 'add':
      if (!empty($field)) {
        return drupal_get_form('sphinx_field_add', $iid, $field);
      }
      else {
        return;
      }
      break;
    
    case 'enable':
      $res = db_query('UPDATE {sphinx_fields} SET active = 1 WHERE fid =%d', $field);
      drupal_goto('admin/settings/sphinx/indexes/fields/'. $iid);
      break;
    
    case 'disable':
      $res = db_query('UPDATE {sphinx_fields} SET active = 0 WHERE fid =%d', $field);
      drupal_goto('admin/settings/sphinx/indexes/fields/'. $iid);
      break;
  }
}

function sphinx_field_confirm_delete($form_state,$iid, $fid) {
  
  
  $result       = db_query('SELECT field_name, display_name FROM {sphinx_fields} WHERE iid = %d && fid= %d;', $iid, $fid);
  $names        = db_fetch_object($result);
  $form         = array();
  $form['text'] = array(
    '#value' => t('Are you sure you want to delete the field <strong>%title</strong> (<i>%display</i>)? ', array('%title' => $names->field_name, '%display' => $names->display_name)),
  );
  $form['iid']    = array('#type' => 'hidden', '#value' => $iid);
  $form['fid']    = array('#type' => 'hidden', '#value' => $fid);
  $form['submit'] = array('#type' => 'submit', '#value' => t('Delete'));
  $form['cancel'] = array('#type' => 'submit', '#value' => t('Cancel'));
  
  return $form;
}

function sphinx_field_confirm_delete_submit($form_id, $form_state) {
  if ($form_state['values']['op'] == 'Delete') {
    db_query('DELETE FROM {sphinx_fields} WHERE iid=%d && fid=%d;', $form_state['values']['iid'], $form_state['values']['fid']);
  }
  drupal_goto('admin/settings/sphinx/indexes/fields/'. $form_state['values']['iid']);
}

function sphinx_field_add($form_state, $iid, $field) {
  if (!empty($iid)) {
    $result = db_query('SELECT index_name, display_name FROM {sphinx_indexes} WHERE iid=%d;', $iid);
    $names = db_fetch_object($result);
  }
  
  $form['sphinx']['text'] = array(
    '#value' => t('You are about to add the field <i>%field</i> to the index <strong>%title</strong> (<i>%display</i>), iid %iid', array('%field' => $field, '%title' => $names->index_name, '%display' => $names->display_name,'%iid'=>$iid)),
  );
  $form['sphinx']['field_name'] = array('#type' => 'hidden', '#value' => $field);
  $form['sphinx']['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display name'),
    '#description' => t('Type the name you want the users to see when using this field'),
    '#required' => true,
  );
  $form['sphinx']['excerpt'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Should this field be in excerpts?'),
  );
  $form['sphinx']['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#description' => t('Choose in which order field should appear in excerpts and in advanced search dropdown'),
  );
  $form['sphinx']['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Should this field be activated?'),
  );
  $form['sphinx']['iid'] = array('#type' => 'hidden', '#value' => $iid);
  $form['sphinx']['submit'] = array('#type' => 'submit', '#value' => t('Submit'));
  return $form;
}

function sphinx_field_add_submit($form_id, $form_state) {
  db_query("INSERT INTO {sphinx_fields} (iid, field_name, display_name, excerpt, weight, active)  VALUES (%d, '%s','%s',%d,%d,%d)", $form_state['values']['iid'], $form_state['values']['field_name'], $form_state['values']['display_name'], $form_state['values']['excerpt'], $form_state['values']['weight'], $form_state['values']['active']);
  //exit;
  drupal_goto('admin/settings/sphinx/indexes/fields/'. $form_state['values']['iid']);
}

function sphinx_field_edit($form_state,$iid, $fid) {
  
  
  if (!empty($iid)) {
    $result = db_query('SELECT index_name, display_name FROM {sphinx_indexes} WHERE iid = %d;', $iid);
    $index = db_fetch_object($result);
  }
  if (!empty($fid)) {
    $result = db_query('SELECT field_name, display_name, excerpt, weight, active FROM {sphinx_fields} WHERE fid = %d;', $fid);
    $field = db_fetch_object($result);
  }
  
  $form['sphinx']['addnew']['text'] = array(
    '#value' => t('You are about to edit the field <i>%field</i> in the index <strong>%title</strong> (<i>%display</i>)', array('%field' => $field->field_name, '%title' => $index->index_name, '%display' => $index->display_name)),
  );
  
  $form['sphinx']['addnew']['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display name'),
    '#description' => t('Type the name you want the users to see when using this field'),
    '#default_value' => $field->display_name,
    '#required' => true,
  );
  $form['sphinx']['addnew']['excerpt'] = array(
    '#type' => 'checkbox',
    '#title' => t('Excerpt'),
    '#default_value' => $field->excerpt,
    '#description' => t('Should this field be in excerpts?'),
  );
  $form['sphinx']['addnew']['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#default_value' => $field->weight,
    '#description' => t('Choose in which order field should appear in excerpts and in advanced search dropdown'),
  );
  $form['sphinx']['addnew']['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Should this field be activated?'),
    '#default_value' => $field->active,
  );
  $form['sphinx']['addnew']['fid'] = array('#type' => 'hidden', '#value' => $fid);
  $form['sphinx']['addnew']['iid'] = array('#type' => 'hidden', '#value' => $iid);
  $form['sphinx']['addnew']['submit'] = array('#type' => 'submit', '#value' => t('Submit'));
  return $form;
}

function sphinx_field_edit_submit($form_id, $form_state) {
  db_query("UPDATE {sphinx_fields} SET display_name = '%s', excerpt=%d, weight=%d, active=%d WHERE fid=%d AND iid=%d", $form_state['values']['display_name'], $form_state['values']['excerpt'], $form_state['values']['weight'], $form_state['values']['active'], $form_state['values']['fid'], $form_state['values']['iid']);
  drupal_goto('admin/settings/sphinx/indexes/fields/'. $form_state['values']['iid']);
}

function _sphinx_manage_attributes($iid) {
  $output = '';
  $sql    = "SELECT * FROM {sphinx_indexes} WHERE iid=%d;";
  $result = db_query($sql, $iid);
  $index  = db_fetch_object($result);
  $client = new SphinxClient();
  $client->SetServer($index->server, (int)$index->port);
  
  $connect = $client->_Connect();
  if (!$connect) {
    return ('Index not available to manage_attributes '.$index->server.':'.$port);
  }
  $client->SetLimits(0, 1);
  $res     = $client->Query('*e*', $index->index_name);
  $serving = $res['attrs'];
  if(module_exists('sphinx_facet')) {
    $header  = array(
      array('data' => t('Attribute'), 'field' => 'attribute_name'),
      array('data' => t('Display name'), 'field' => 'display_name'),
      array('data' => t('Sort'), 'field' => 'sort'),
      array('data' => t('Active'), 'field' => 'active'),
      array('data' => t('Facet'), 'field' => 'facet'),
      array('data' => t('Type'), 'field' => 'type'),
      array('data' => t('Served')),
      array(),
      array(),
    );
  } else {
    $header  = array(
      array('data' => t('Attribute'), 'field' => 'attribute_name'),
      array('data' => t('Display name'), 'field' => 'display_name'),
      array('data' => t('Sort'), 'field' => 'sort'),
      array('data' => t('Active'), 'field' => 'active'),
      array('data' => t('Served')),
      array(),
      array(),
    );
  
  }
  
  $sql     = 'SELECT * FROM {sphinx_attributes} WHERE iid=%d '. tablesort_sql($header);
  $result  = db_query($sql, $iid);
  $counter = 0;
  while ($attributes = db_fetch_object($result)) {
    $counter++;
    if (array_key_exists($attributes->attribute_name, $serving)) {
      unset($serving[$attributes->attribute_name]);
      $status = 'Serving';
    }
    else {
      $status = '<span class="error">Not served!</span>';
    }
    $active = ($attributes->active == 0) ? l(t('Enable'), 'admin/settings/sphinx/attributes/enable/'. $iid .'/'. $attributes->aid) : l(t('Disable'), 'admin/settings/sphinx/attributes/disable/'. $iid .'/'. $attributes->aid);
    if(module_exists('sphinx_facet')) {
      $rows[] = array(
        $attributes->attribute_name,
        $attributes->display_name,
        $attributes->sort,
        $attributes->active ? 'Enabled' : 'Disabled',
        $attributes->facet ? 'Enabled' : 'Disabled',
        $attributes->type,
        $status,
        l(t('Delete'), 'admin/settings/sphinx/attributes/delete/'. $iid .'/'. $attributes->aid),
        $active,
        l(t('Edit'), 'admin/settings/sphinx/attributes/edit/'. $iid .'/'. $attributes->aid),
      );
    } else {
      $rows[] = array(
        $attributes->attribute_name,
        $attributes->display_name,
        $attributes->sort,
        $attributes->active ? 'Enabled' : 'Disabled',
        $status,
        l(t('Delete'), 'admin/settings/sphinx/attributes/delete/'. $iid .'/'. $attributes->aid),
        $active,
        l(t('Edit'), 'admin/settings/sphinx/attributes/edit/'. $iid .'/'. $attributes->aid),
      );

    
    }
  }
  if ($counter) {
    $output .= theme('table', $header, $rows);
  }
  if (count($serving)) {
    
    $add_header = array(
      array('data' => t('Attribute')),
      array(),
    );
    foreach ($serving as $attribute => $value) {
      $add_rows[] = array($attribute, l(t('Add'), 'admin/settings/sphinx/attributes/add/'. $iid .'/'. $attribute));
    }
    $output .= theme('table', $add_header, $add_rows);
  }
  return $output;
}

function _sphinx_alter_attributes($op, $iid, $attribute) {
  switch ($op) {
    case 'delete':
      return drupal_get_form('sphinx_attribute_confirm_delete', $iid, $attribute);
    break;
    
    case 'edit':
      return drupal_get_form('sphinx_attribute_edit', $iid, $attribute);
    break;
    
    case 'add':
      if (!empty($attribute)) {
        return drupal_get_form('sphinx_attribute_add', $iid, $attribute);
      }
      else {
        return;
      }
      break;
    
    case 'enable':
      $res = db_query('UPDATE {sphinx_attributes} SET active = 1 WHERE aid =%d', $attribute);
      drupal_goto('admin/settings/sphinx/indexes/attributes/'. $iid);
      break;
    
    case 'disable':
      $res = db_query('UPDATE {sphinx_attributes} SET active = 0 WHERE aid =%d', $attribute);
      drupal_goto('admin/settings/sphinx/indexes/attributes/'. $iid);
      break;
  }
}

function sphinx_attribute_confirm_delete($form_state,$iid, $aid) {
  
  
  $result       = db_query('SELECT attribute_name, display_name FROM {sphinx_attributes} WHERE iid = %d && aid= %d;', $iid, $aid);
  $names        = db_fetch_object($result);
  $form         = array();
  $form['text'] = array(
    '#value' => t('Are you sure you want to delete the attribute <strong>%title</strong> (<i>%display</i>)? ', array('%title' => $names->attribute_name, '%display' => $names->display_name)),
  );
  $form['iid']    = array('#type' => 'hidden', '#value' => $iid);
  $form['aid']    = array('#type' => 'hidden', '#value' => $aid);
  $form['submit'] = array('#type' => 'submit', '#value' => t('Delete'));
  $form['cancel'] = array('#type' => 'submit', '#value' => t('Cancel'));
  
  return $form;
}

function sphinx_attribute_confirm_delete_submit($form_id, $form_state) {
  if ($form_state['values']['op'] == 'Delete') {
    db_query('DELETE FROM {sphinx_attributes} WHERE iid=%d && aid=%d;', $form_state['values']['iid'], $form_state['values']['aid']);
  }
  drupal_goto('admin/settings/sphinx/indexes/attributes/'. $form_state['values']['iid']);
}

function sphinx_attribute_add($form_state,$iid, $attribute) {
  
  
  if (!empty($iid)) {
    $result = db_query('SELECT index_name, display_name FROM {sphinx_indexes} WHERE iid = %d;', $iid);
    $names = db_fetch_object($result);
  }
  
  $form['sphinx']['addnew']['text'] = array(
    '#value' => t('You are about to add the attribute <i>%attribute</i> to the index <strong>%title</strong> (<i>%display</i>)', array('%attribute' => $attribute, '%title' => $names->index_name, '%display' => $names->display_name)),
  );
  $form['sphinx']['addnew']['attribute_name'] = array('#type' => 'hidden', '#value' => $attribute);
  $form['sphinx']['addnew']['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display name'),
    '#description' => t('Type the name you want the users to see when using this attribute'),
    '#required' => true,
  );
  
  $form['sphinx']['addnew']['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Should this attribute be activated?'),
  );
  if(module_exists('sphinx_facet')) {
    $form['sphinx']['addnew']['facet'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Should this attribute be used as a facet?'),
    );
    $form['sphinx']['addnew']['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array('term' => t('Term'), 'user' => t('User'), 'node' => t('Node'), 'date' => t('Date'), 'other' => t('Other')),
    '#description' => t('Should this attribute be used as a facet?'),
    );
  }
  $form['sphinx']['addnew']['iid'] = array('#type' => 'hidden', '#value' => $iid);
  $form['sphinx']['addnew']['submit'] = array('#type' => 'submit', '#value' => t('Submit'));
  return $form;
}

function sphinx_attribute_add_submit($form_id, $form_state) {
  $facet = (empty($form_state['values']['facet']))?0:$form_state['values']['facet'];
  $type = (empty($form_state['values']['type']))?'':$form_state['values']['type'];
  db_query("INSERT INTO {sphinx_attributes} (iid, attribute_name, display_name, active, facet, type)  VALUES (%d, '%s','%s',%d, %d, '%s')", $form_state['values']['iid'], $form_state['values']['attribute_name'], $form_state['values']['display_name'], $form_state['values']['active'], $facet, $type);
  drupal_goto('admin/settings/sphinx/indexes/attributes/'. $form_state['values']['iid']);
}

function sphinx_attribute_edit($form_state,$iid, $aid) {
  
  
  if (!empty($iid)) {
    $result = db_query('SELECT index_name, display_name FROM {sphinx_indexes} WHERE iid = %d;', $iid);
    $index = db_fetch_object($result);
  }
  if (!empty($aid)) {
    $result = db_query('SELECT attribute_name, display_name, active, facet, type FROM {sphinx_attributes} WHERE aid = %d;', $aid);
    $attribute = db_fetch_object($result);
  }
  
  $form['sphinx']['addnew']['text'] = array(
    '#value' => t('You are about to edit the attribute <i>%attribute</i> in the index <strong>%title</strong> (<i>%display</i>)', array('%attribute' => $attribute->attribute_name, '%title' => $index->index_name, '%display' => $index->display_name)),
  );
  
  $form['sphinx']['addnew']['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display name'),
    '#description' => t('Type the name you want the users to see when using this attribute'),
    '#default_value' => $attribute->display_name,
    '#required' => true,
  );
  
  $form['sphinx']['addnew']['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Should this attribute be activated?'),
    '#default_value' => $attribute->active,
  );
   if(module_exists('sphinx_facet')) {
    $form['sphinx']['addnew']['facet'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Should this attribute be used as a facet?'),
    '#default_value' => $attribute->facet,
    );
    $form['sphinx']['addnew']['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array('term' => t('Term'), 'user' => t('User'), 'node' => t('Node'), 'date' => t('Date'), 'other' => t('Other')),
    '#description' => t('Should this attribute be used as a facet?'),
    '#default_value' => $attribute->type,

    );
  }
  $form['sphinx']['addnew']['aid'] = array('#type' => 'hidden', '#value' => $aid);
  $form['sphinx']['addnew']['iid'] = array('#type' => 'hidden', '#value' => $iid);
  $form['sphinx']['addnew']['submit'] = array('#type' => 'submit', '#value' => t('Submit'));
  return $form;
}

function sphinx_attribute_edit_submit($form_id, $form_state) {
  if(module_exists('sphinx_facet')) {
    db_query("UPDATE {sphinx_attributes} SET display_name = '%s', active=%d, facet=%d, type='%s' WHERE aid=%d AND iid=%d", $form_state['values']['display_name'], $form_state['values']['active'], $form_state['values']['facet'], $form_state['values']['type'], $form_state['values']['aid'], $form_state['values']['iid']);
  } else {
    db_query("UPDATE {sphinx_attributes} SET display_name = '%s', active=%d WHERE aid=%d AND iid=%d", $form_state['values']['display_name'], $form_state['values']['active'], $form_state['values']['aid'], $form_state['values']['iid']);
  }
  drupal_goto('admin/settings/sphinx/indexes/attributes/'. $form_state['values']['iid']);
}

function _sphinx_get_field_display_name_by_iid($fid = null) {
  if (!empty($iid)) {
    $res = db_query("SELECT display_name FROM {sphinx_fields} WHERE fid=%d", $fid);
    $index = db_fetch_object($res);
    return $index->display_name;
  }
}

