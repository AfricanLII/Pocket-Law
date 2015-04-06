<?php

function _seotools_presets() {
  return array(
    'seotools' => array(
      'title' => 'SEO Tools',
      'version' => 1,
      'description' => 'Presets for SEO optimization',
      'file' => 'plugins/seotools.presets.inc',
      'modules_footer' => t('!seotools created by !levelten',
        array(
          '!seotools' => l('SEO Tools', 'http://www.leveltendesign.com/seo-tools'),
          '!levelten' => l('LevelTen Interactive', 'http://www.leveltendesign.com'),
        )
      ),
      'modules' => array(
        'seotools' => array(
          'name' => 'SEO Tools',
          'footer' => 'To change the settings, go to ' . l('admin/settings/seotools', 'admin/settings/seotools'),
          'settings' => array(
            'seotools_feature',
          ),
        ),
        'tutorials' => array(
          'name' => 'Tutorials',
        ),
        'path' => array(
          'name' => 'Path (Core)',
        ),
        'pathauto' => array(
          'name' => 'Pathauto',
        ),
        'taxonomy' => array(
          'name' => 'Taxonomy (Core)',
        ),
        'token' => array(
          'name' => 'Token',
        ),
        'features' => array(
          'name' => 'Features',
        ),
        'strongarm' => array(
          'name' => 'Strongarm',
        ),
        'googleanalytics' => array(
          'project' => 'google_analytics',
          'name' => 'Google Analytics',
          'version' => '6.x-3.3',
          'settings' => array(
            'googleanalytics_account',
          ),
          'footer' => 'To change the settings, go to ' . l('admin/settings/googleanalytics', 'admin/settings/googleanalytics'),
        ),
        'google_analytics_reports' => array(
          'name' => 'Google Analytics Reports',
          'version' => '6.x-1.0',
          'submodules' => array(
            'google_analytics_api',
          ),
          'settings' => array(
            'profile_id',
          ),
          'footer' => 'To change the settings, go to ' . l('admin/settings/google-analytics-reports', 'admin/settings/google-analytics-reports'),
        ),
        'chart' => array(
          'name' => 'Chart',
          'version' => '6.x-1.3',
          'submodules' => array(
            'system_charts',
          ),
        ),
        'countries_api' => array(
          'name' => 'Countries API',
          'version' => '6.x-1.1',
        ),
        'seo_friend' => array(
          'name' => 'SEO Friend',
          'version' => '6.x-1.1',
        ),
        'wordstream' => array(
          'name' => 'Wordstream',
          'version' => '6.x-1.0-beta3',
          'footer' => 'To change the settings, go to ' . l('admin/settings/wordstream', 'admin/settings/wordstream'),
          'settings' => array(
            'wordstream_username',
            'wordstream_free_embed_id',
          ),
        ),
        'nodewords' => array(
          'name' => 'Node Words',
          'version' => '6.x-1.11',
          'submodules' => array(
            'nodewords_extra',
            'nodewords_verification_tags',
            'nodewords_basic',
          ),
        ),
        'page_title' => array(
          'name' => 'Page Title',
          'version' => '6.x-2.5',
          'settings' => array(
            'nodes',
          ),
          'footer' => 'To change the settings, go to ' . l('admin/content/page_title', 'admin/content/page_title'),
        ),
        'globalredirect' => array(
          'name' => 'Global Redirect',
          'version' => '6.x-1.2',
        ),
        'path_redirect' => array(
          'name' => 'Path Redirect',
          'version' => '6.x-1.0-rc2',
        ),
        'xmlsitemap' => array(
          'name' => 'XML Sitemap',
          'version' => '6.x-2.0-beta3',
          'submodules' => array(
            'xmlsitemap_custom',
            'xmlsitemap_node',
            'xmlsitemap_engines',
            'xmlsitemap_user',
            'xmlsitemap_menu',
            'xmlsitemap_taxonomy',
           ),
          'settings' => array(
            'vocabularies',
            'content_types',
            'menus',
          ),
          'footer' => 'To change the settings, go to ' . l('admin/settings/xmlsitemap', 'admin/settings/xmlsitemap'),
        ),
        'contentanalysis' => array(
          'name' => 'Content Analysis',
          'version' => '6.x-1.6',
        ),
        'contentoptimizer' => array(
          'name' => 'Content Optimizer',
          'version' => '6.x-2.2',
        ),
        'alchemy' => array(
          'name' => 'Alchemy',
          'version' => '6.x-1.0-rc6',
          'submodules' => array(
            'alchemy_contentanalysis',
          ),
          'settings' => array(
            'alchemy_apikey',
          ),
          'footer' => 'To change the settings, go to ' . l('admin/settings/alchemy', 'admin/settings/alchemy'),
        ),
        'kwresearch' => array(
          'name' => 'Keyword Research',
          'version' => '6.x-1.0-rc9',
          'submodules' => array(
            'kwresearch_google',
          ),
          'settings' => array(
            'kwresearch_apikey',
            'kwresearch_keyword_sync_vocabulary',
          ),
          'footer' => 'To change the settings, go to ' . l('admin/settings/kwresearch', 'admin/settings/kwresearch'),
        ),
        //'w3c_validator' => array(
        //  'name' => 'W3C Validator',
        //  'version' => '6.x-1.2',
        //),
        'linkintel' => array(
          'name' => 'Link Intelligence',
          'version' => '6.x-1.0-rc11',
          'submodules' => array(
            'linkintel_cck',
            'linkintel_stemmer',
          ),
          'settings' => array(
            'request_sync_tax_vocabs',
            'suggestions_seed_vocabs',
          ),
          'footer' => 'To change the settings, go to ' . l('admin/settings/linkintel', 'admin/settings/linkintel'),
        ),
      ),
    ),
  );
}

function seotools_seotools_seotools_feature() {
  if (module_exists('features')) {
    module_load_include('inc', 'features', 'features.export');
    $status = features_get_storage('seotools');
    $setting = array();
    $setting['title'] = 'SEO Feature';
    if ($status == 0) {
      $setting['value'] = 'Feature is default';
      $setting['severity'] = 0;
    }
    else {
      $setting['value'] = 'Feature is overridden';
      $setting['description'] = l('Review overrides' , 'admin/build/features/seotools');
      $setting['severity'] = REQUIREMENT_ERROR;
    }

    return $setting;
  }
}

function seotools_seotools_seotools_feature_reset() {
  module_load_include('inc', 'features', 'features.export');
  features_include();
  $module = 'seotools';
  $seotools = features_get_features($module);
  $revert = array();
  foreach ($seotools->info['features'] as $component => $status) {
    $revert[$module][] = $component;
    drupal_set_message(t('Reverted all <strong>!component</strong> components for <strong>!module</strong>.', array('!component' => $component, '!module' => $module)));
  }
  features_revert($revert);

}

function seotools_google_analytics_reports_profile_id() {
  $setting = array();
  $setting['title'] = 'Google Analytics Profile';
  if (variable_get('google_analytics_reports_profile_id', 0) != 0) {
    $setting['value'] = 'Your google analytics account has been set.';
    //$setting['value'] = variable_get('google_analytics_reports_profile_id', 0);
    $setting['severity'] = 0;
  }
  else {
    $setting['value'] = 'You have not set your google analytics account. Go to ' . l('admin/settings/google-analytics-reports', 'admin/settings/google-analytics-reports') . ' to set up your account.';
    $setting['severity'] = REQUIREMENT_ERROR;
  }

  return $setting;
}
function seotools_page_title_nodes() {
  $titles_set = array();
  $titles_not_set = array();
  $types = node_get_types();
  foreach ($types as $type) {
    // Define the node-type key
    $key = 'page_title_type_'. $type->type . '_showfield';
    if (variable_get($key, 0)) {
      $titles_set[$type->type] = $type->name;
    }
    else {
      $titles_not_set[$type->type] = $type->name;
    }
  }
  $setting = array();
  $setting['title'] = 'Node Page Titles';
  if (empty($titles_set)) {
    $setting['value'] = 'No content types are set to use page titles';
    $setting['severity'] = REQUIREMENT_ERROR;
  }
  elseif (!empty($titles_not_set)) {
    $account = user_load(variable_get('seotools_uid', 0));
    $setting['value'] = 'Not using Page Titles: ' . implode(', ', $titles_not_set);
    $setting['severity'] = REQUIREMENT_WARNING;
  }
  else {
    $account = user_load(variable_get('seotools_uid', 0));
    $setting['value'] = 'All content types are set up to use page titles.';
    $setting['severity'] = REQUIREMENT_OK;
  }

  return $setting;
}

function seotools_page_title_nodes_reset() {
  $types = node_get_types();
  foreach ($types as $type) {
    // Define the node-type key
    $key = 'page_title_type_'. $type->type . '_showfield';
    variable_set($key, 1);
  }
}

function seotools_xmlsitemap_vocabularies() {
  $vocabs = taxonomy_get_vocabularies();
  $setting = array();
  $setting['title'] = 'Vocabularies';
  $settings = array(
    'status' => '1',
    'priority' => '0.5',
  );
  $different = array();
  foreach($vocabs as $vid => $vocab) {
    if ((array_diff(variable_get('xmlsitemap_settings_taxonomy_term_' . $vid, array(FALSE)), $settings))) {
      $different[$vid] = $vocab->name;
    }
  }
  if (empty($different)) {
    $setting['value'] = 'All taxonomies are set for xmlsitemap.';
    $setting['severity'] = REQUIREMENT_OK;
  }
  else {
    $setting['value'] = 'No vocabularies not set to use xmlsitemap.';
    $setting['severity'] = REQUIREMENT_ERROR;
  }

  return $setting;
}

function seotools_xmlsitemap_vocabularies_reset() {
  $vocabs = taxonomy_get_vocabularies();
  $settings = array(
    'status' => '1',
    'priority' => '0.5',
  );
  foreach($vocabs as $vid => $vocab) {
    variable_set('xmlsitemap_settings_taxonomy_term_' . $vid, $settings);
  }
}

function seotools_xmlsitemap_content_types() {
  $types = node_get_types();
  $setting = array();
  $setting['title'] = 'Content Types';
  $settings = array(
    'status' => '1',
    'priority' => '0.5',
  );
  $different = array();
  foreach($types as $type => $info) {
    if ((array_diff(variable_get('xmlsitemap_settings_node_' . $type, array(FALSE)), $settings))) {
      $different[$type] = $info->name;
    }
  }
  if (empty($different)) {
    $setting['value'] = 'All content types are set for xmlsitemap.';
    $setting['severity'] = REQUIREMENT_OK;
  }
  elseif (count($different) != count($types)) { // If some content types are set, only post a warning.
    $setting['value'] = implode($different, ', ' ) . ' content types not set to use xmlsitemaps.';
    $setting['severity'] = REQUIREMENT_WARNING;
  }
  else {
    $setting['value'] = 'No content types not set to use xmlsitemaps.';
    $setting['severity'] = REQUIREMENT_ERROR;
  }

  return $setting;
}

function seotools_xmlsitemap_content_types_reset() {
  $types = node_get_types();
  $settings = array(
    'status' => '1',
    'priority' => '0.5',
  );
  foreach($types as $type => $info) {
    variable_set('xmlsitemap_settings_node_' . $type, $settings);
  }
}

function seotools_xmlsitemap_menus() {
  $types = menu_get_menus();
  $setting = array();
  $setting['title'] = 'Menus';
  $settings = array(
    'status' => '1',
    'priority' => '0.5',
  );
  $different = array();
  foreach($types as $type => $info) {
    if ((array_diff(variable_get('xmlsitemap_settings_menu_' . $type, array(FALSE)), $settings))) {
      $different[$type] = $info;
    }
  }
  if (empty($different)) {
    $setting['value'] = 'All menus are set for xmlsitemap.';
    $setting['severity'] = REQUIREMENT_OK;
  }
  elseif (count($different) != count($types)) { // If some content types are set, only post a warning.
    $setting['value'] = implode($different, ', ' ) . ' menus not set to use xmlsitemaps.';
    $setting['severity'] = REQUIREMENT_WARNING;
  }
  else {
    $setting['value'] = 'No meus not set to use xmlsitemaps.';
    $setting['severity'] = REQUIREMENT_ERROR;
  }

  return $setting;
}

function seotools_xmlsitemap_menus_reset() {
  $types = menu_get_menus();
  $settings = array(
    'status' => '1',
    'priority' => '0.5',
  );
  foreach($types as $type => $info) {
    variable_set('xmlsitemap_settings_menu_' . $type, $settings);
  }
}

function seotools_googleanalytics_googleanalytics_account() {
  $setting = array();
  $setting['title'] = 'Account Key';
  if (variable_get('googleanalytics_account', 'UA-') == 'UA-') {
    $setting['value'] = drupal_get_form('seotools_googleanalytics_googleanalytics_account_form');
    $setting['severity'] = REQUIREMENT_ERROR;
  }
  else {
    $setting['value'] = 'Google Analytics Account Key set to: ' . variable_get('googleanalytics_account', '');
    $setting['severity'] = REQUIREMENT_OK;
  }

  return $setting;
}

function seotools_googleanalytics_googleanalytics_account_form() {
  $form['account']['googleanalytics_account'] = array(
    '#type' => 'textfield',
    '#title' => t('Google Analytics account number'),
    '#default_value' => variable_get('googleanalytics_account', 'UA-'),
    '#size' => 15,
    '#maxlength' => 20,
    '#required' => TRUE,
    '#description' => t('The account number is unique to the websites domain. Click the <strong>Edit</strong> link in your Google Analytics account next to the appropriate profile on the <br /><strong>Analytics Settings</strong> page, then select <strong>Check Status</strong> at the top-right of the table to find the account number (UA-xxxx-x)<br /> of your site. You can obtain a user account from the <a href="@url">Google Analytics</a> website.', array('@url' => 'http://www.google.com/analytics/')),
  );
  return system_settings_form($form);
}

function seotools_googleanalytics_googleanalytics_account_form_validate($form, &$form_state) {
  if (!preg_match('/^UA-\d{4,}-\d+$/', $form_state['values']['googleanalytics_account'])) {
    form_set_error('googleanalytics_account', t('A valid Google Analytics account number is case sensitive and formatted like UA-xxxxxx-x.'));
  }
}


function seotools_alchemy_alchemy_apikey() {
  $setting = array();
  $setting['title'] = 'API Key';
  if (variable_get('alchemy_apikey', '') == '') {
    $setting['value'] = drupal_get_form('seotools_alchemy_alchemy_apikey_form');
    $setting['severity'] = REQUIREMENT_ERROR;
  }
  else {
    $setting['value'] = 'Alchemy API Key set to: ' . variable_get('alchemy_apikey', '');
    $setting['severity'] = REQUIREMENT_OK;
  }

  return $setting;
}

function seotools_alchemy_alchemy_apikey_form() {
  $form['alchemy_apikey'] = array(
    '#type' => 'textfield',
    '#title' => t('Alchemy API Key'),
    '#default_value' => variable_get('alchemy_apikey', ''),
    '#description' => t('You need an API key to use the Alchemy API service. !alchemy_link.',
      array(
        '!alchemy_link' => l(t('Get an Alchemy API key here'), 'http://www.alchemyapi.com/api/register.html', array('attributes' => array('target' => '_blank'))),
      )
    ),
  );
  return system_settings_form($form);
}

function seotools_wordstream_wordstream_username() {
  $setting = array();
  $setting['title'] = 'Wordstream API Account';
  if (variable_get('wordstream_username', '') == '' || variable_get('wordstream_password', '') == '') {
    $setting['value'] = drupal_get_form('seotools_wordstream_wordstream_username_form');
    $setting['severity'] = REQUIREMENT_ERROR;
  }
  else {
    $setting['value'] = 'Wordstream API Username set to: ' . variable_get('wordstream_username', '');
    $setting['severity'] = REQUIREMENT_OK;
  }

  return $setting;
}

function seotools_wordstream_wordstream_username_form() {
  $form['wordstream_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Wordstream API Username'),
    '#default_value' => variable_get('wordstream_username', ''),
  );
  $form['wordstream_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Wordstream API Password'),
    '#default_value' => variable_get('wordstream_password', ''),
    '#description' => t('In order to enable the API tools you will need a WordStream account login (username & password). !wordstream_link.',
      array(
        '!wordstream_link' => l(t('Get your API account here'), WORDSTREAM_LINK_API_ACCOUNT, array('attributes' => array('target', 'wordstream'))),
      )
    ),
  );
  return system_settings_form($form);
}

/* Wordstream no longer supports free embed id
function seotools_wordstream_wordstream_free_embed_id() {
  $setting = array();
  $setting['title'] = 'Free widgets embed id';
  if (variable_get('wordstream_free_embed_id', '') == '') {
    $setting['value'] = drupal_get_form('seotools_wordstream_wordstream_free_embed_id_form');
    $setting['severity'] = REQUIREMENT_ERROR;
  }
  else {
    $setting['value'] = 'Wordstream Free widgets embed id: ' . variable_get('wordstream_free_embed_id', '');
    $setting['severity'] = REQUIREMENT_OK;
  }

  return $setting;
}

function seotools_wordstream_wordstream_free_embed_id_form() {
  $form['wordstream_free_embed_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Free widgets embed id'),
    '#description' => t(''),
    '#default_value' => variable_get('wordstream_free_embed_id', ''),
    '#description' => t('In order to enable the free javascript tools you will need a WordStream widget embed id. <br>The embed id is a part of the widget embed code that you can generate on the WordStream site. <br>Enter in the field above the 20 character value following "embed_id=" in the widget embed code.<br> !wordstream_link.',
      array(
        '!wordstream_link' => l(t('Get your embed code here'), WORDSTREAM_LINK_WIDGET_GENERATOR, array('attributes' => array('target', 'wordstream'))),
      )
    )
  );
  return system_settings_form($form);
}
*/

function seotools_kwresearch_kwresearch_apikey() {
  $setting = array();
  $setting['title'] = 'API Key';
  if (variable_get('kwresearch_apikey', '') == '') {
    $setting['value'] = drupal_get_form('seotools_kwresearch_kwresearch_apikey_form');
    $setting['severity'] = REQUIREMENT_ERROR;
  }
  else {
    $setting['value'] = 'Keyword Reasearch API Key set to: ' . variable_get('kwresearch_apikey', '');
    $setting['severity'] = REQUIREMENT_OK;
  }

  return $setting;
}

function seotools_kwresearch_kwresearch_apikey_form() {
  $form['kwresearch_apikey'] = array(
    '#type' => 'textfield',
    '#title' => t('Wordtracker API Key'),
    '#default_value' => variable_get('kwresearch_apikey', ''),
    '#description' => t('You need an API key to enable keyword statistics and related phrase lookups. <br>If you have a Wordtracker API key enter it above. <br>To get a full API key, contact !wordtracker_link. You can also use a free !levelten_link <br>to enable demo level access to Wordtracker statitics.',
      array(
        '!wordtracker_link' => l(t('Wordtracker'), 'http://www.wordtracker.com/api', array('attributes' => array('target' => '_blank'))),
        '!levelten_link' => l(t('LevelTen SEO API key'), 'http://www.leveltendesign.com/user/0/l10seoapi', array('attributes' => array('target' => '_blank'))),
      )
    ),
  );
  return system_settings_form($form);
}

function seotools_kwresearch_kwresearch_keyword_sync_vocabulary() {
  $setting = array();
  $setting['title'] = 'Keyword Vocabulary';
  if (variable_get('kwresearch_keyword_sync_vocabulary', 0) == 0) {
    $setting['value'] = drupal_get_form('seotools_kwresearch_kwresearch_keyword_sync_vocabulary_form');
    $setting['severity'] = REQUIREMENT_ERROR;
  }
  else {
    $vocab = taxonomy_vocabulary_load(variable_get('kwresearch_keyword_sync_vocabulary', 0));
    $setting['value'] = 'Vocabulary set to: ' . $vocab->name . ' (' . $vocab->vid . ')';
    $setting['severity'] = 0;
  }

  return $setting;
}

function seotools_kwresearch_kwresearch_keyword_sync_vocabulary_form() {
  // Populate the vocab_options array
  $vocabs = taxonomy_get_vocabularies();
  $vocab_options = array(0 => '<none>');
  foreach ($vocabs as $id => $vocab) {
    $vocab_options[$vocab->vid] = $vocab->name;
  }

  $form['kwresearch_keyword_sync_vocabulary'] = array(
    '#type' => 'select',
    '#title' => 'Vocabulary for Autotagging Keywords',
    '#options' => $vocab_options,
    '#default_value' => variable_get('kwresearch_keyword_sync_vocabulary', 0),
    '#description' => '<div>In order to use autotagging, it must be associated with a vocabulary.  Please select a vocabulary to use.</div>',
  );
  return system_settings_form($form);
}

function seotools_kwresearch_kwresearch_keyword_sync_vocabulary_reset() {
  // First search if the vocabulary has already been created.
  $vocabularies = taxonomy_get_vocabularies();
  if (is_array($vocabularies)) {
    foreach($vocabularies as $vid => $info) {
      if ($info->name == 'Keywords') {
        $tags = (array) $info;
      }
    }
  }
  // If it doesn't exist yet, create it.
  if (!isset($tags)) {
    $tags = array(
      'name' => 'Keywords',
      'description' => 'generated vocabulary for keyword content',
      'tags' => 1,
      'relations' => 1,
      'module' => 'taxonomy',
      'nodes' => array_combine(array_keys(node_get_types()), array_keys(node_get_types())),
    );
    taxonomy_save_vocabulary($tags);
    drupal_set_message('A keyword vocabulary was generated for you.');
  }
  // Set the variables for the tags vocabulary.
  variable_set('kwresearch_keyword_sync_vocabulary', $tags['vid']);
  $report_vocabs = variable_get('kwresearch_report_vocabulary', array());
  $report_vocabs[$tags['vid']] = $tags['vid'];
  variable_set('kwresearch_report_vocabulary', $report_vocabs);
}

function seotools_linkintel_request_sync_tax_vocabs() {
  $setting = array();
  $setting['title'] = 'LinkIntel Sync Vocabularies';
  if (variable_get('linkintel_request_sync_tax_vocabs', array()) == array()) {
    $setting['value'] = drupal_get_form('seotools_linkintel_request_sync_tax_vocabs_form');
    $setting['severity'] = REQUIREMENT_ERROR;
  }
  else {
    $vocabs = variable_get('linkintel_request_sync_tax_vocabs', array());
    $vocab_names = array();
    foreach($vocabs as $vid) {
      $vocab = taxonomy_vocabulary_load($vid);
      $vocab_names[] = $vocab->name;
    }
    $setting['value'] = 'Sync Vocabularies: ' . implode(', ', $vocab_names);
    $setting['severity'] = 0;
  }

  return $setting;
}

function seotools_linkintel_request_sync_tax_vocabs_form() {
  $vocabs = taxonomy_get_vocabularies();
  $options = array();
  if(!empty($vocabs)) {
    foreach ($vocabs AS $vid => $v) {
      $options[$vid] = $v->name;
    }
    $form['tax_vocabs']['linkintel_request_sync_tax_vocabs'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => (isset($settings->request_sync_tax_vocabs)) ? $settings->request_sync_tax_vocabs : array(),
      '#description' => t('Select vocabularies to sync requests.')
    );
  }
  else {
    $form['tax_vocabs']['linkintel_request_sync_tax_vocabs'] = array(
      '#type' => 'item',
      '#value' => t('There are currently no taxonomy vocabularies to sync. !link',
        array(
          '!link' => l(t('Click here to create a vocabulary.'), 'admin/content/taxonomy/add/vocabulary'),
        )
      ),
    );
  }
  return system_settings_form($form);
}

function seotools_linkintel_suggestions_seed_vocabs() {
  $setting = array();
  $setting['title'] = 'LinkIntel Seed Vocabularies';
  if (variable_get('linkintel_suggestions_seed_vocabs', array()) == array()) {
    $setting['value'] = drupal_get_form('seotools_linkintel_suggestions_seed_vocabs_form');
    $setting['severity'] = REQUIREMENT_ERROR;
  }
  else {
    $vocabs = variable_get('linkintel_suggestions_seed_vocabs', array());
    $vocab_names = array();
    foreach($vocabs as $vid) {
      $vocab = taxonomy_vocabulary_load($vid);
      $vocab_names[] = $vocab->name;
    }
    $setting['value'] = 'Seed Vocabularies: ' . implode(', ', $vocab_names);
    $setting['severity'] = 0;
  }

  return $setting;
}

function seotools_linkintel_suggestions_seed_vocabs_form() {
  $vocabs = taxonomy_get_vocabularies();
  $options = array();
  if(!empty($vocabs)) {
    foreach ($vocabs AS $vid => $v) {
        $options[$vid] = $v->name;
    }
    $form['seeding']['linkintel_suggestions_seed_vocabs'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Seed vocabularies'),
      '#options' => $options,
      '#default_value' => variable_get('linkintel_suggestions_seed_vocabs', array()),
      '#description' => t('Select vocabularies to gather terms for seeding the link suggestions tool.')
    );
  }
  else {
    $form['tax_vocabs']['linkintel_suggestions_seed_vocabs'] = array(
      '#type' => 'item',
      '#value' => t('There are currently no taxonomy vocabularies to sync. !link',
        array(
          '!link' => l(t('Click here to create a vocabulary.'), 'admin/content/taxonomy/add/vocabulary'),
        )
      ),
    );
  }
  return system_settings_form($form);
}

