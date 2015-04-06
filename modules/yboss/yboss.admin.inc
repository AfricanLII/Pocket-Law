<?php
// $Id: yboss.admin.inc,v 1.1.2.1 2008/07/14 06:59:28 mfb Exp $

/**
 * Configuration page for Windows Live search.
 */
function yboss_settings() {
  $form = array();
  $form['yboss_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of results per page'),
    '#description' => t('Enter the number of results to display on each page.'),
    '#default_value' => variable_get('yboss_limit', 15),
    '#maxlength' => 4,
    '#size' => 8,
  );
  $form['yboss_appid'] = array(
    '#type' => 'textfield',
    '#title' => t('Yahoo BOSS App ID'),
    '#description' => t('Enter a valid <a href="http://developer.yahoo.com/wsregapp/">Yahoo application ID</a>.'),
    '#default_value' => variable_get('yboss_appid', ''),
    '#maxlength' => 60,
    '#size' => 60,
  );
  $form['yboss_query'] = array(
    '#type' => 'textfield',
    '#title' => t('Appended query string'),
    '#description' => t('You may append an additional query string to the search query.  For example, <q>site:drupal.org -site:cvs.drupal.org</q>.'),
    '#default_value' => variable_get('yboss_query', ''),
    '#maxlength' => 640,
    '#size' => 80,
  );
  $form['yboss_block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Block search box'),
    '#description' => t('Use Yahoo BOSS as the default search engine for the block-based search box.'),
    '#default_value' => variable_get('yboss_block', 0),
  );
  $form['yboss_theme'] = array(
    '#type' => 'checkbox',
    '#title' => t('Theme search box'),
    '#description' => t('Use Yahoo BOSS as the default search engine for the theme-based search box.'),
    '#default_value' => variable_get('yboss_theme', 0),
  );
  $form['yboss_lang'] = array(
    '#type' => 'textfield',
    '#title' => t('Language'),
    '#description' => t('Specify a <a href="http://developer.yahoo.com/search/boss/boss_guide/supp_regions_lang.html">valid language code</a>.'),
    '#default_value' => variable_get('yboss_lang', 'en'),
    '#maxlength' => 2,
    '#size' => 5,
  );
  $form['yboss_region'] = array(
    '#type' => 'textfield',
    '#title' => t('Region'),
    '#description' => t('Specify a <a href="http://developer.yahoo.com/search/boss/boss_guide/supp_regions_lang.html">valid region (country) code</a>.'),
    '#default_value' => variable_get('yboss_region', 'us'),
    '#maxlength' => 2,
    '#size' => 5,
  );
  $form['yboss_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Search tab'),
    '#description' => t('Enter a name for the Yahoo BOSS search tab (defaults to <em>Yahoo</em>).'),
    '#default_value' => variable_get('yboss_name', ''),
    '#size' => 60,
  );
  return system_settings_form($form);
}
