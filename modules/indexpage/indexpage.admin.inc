<?php
// $Id: indexpage.admin.inc,v 1.1.2.4 2009/07/17 18:45:49 nancyw Exp $

/** 
 * @file
 * This module displays customizable index pages for each node type, with
 * alphabetical, taxonomy, and user filters.
 */

/**
 * Implementation of hook_form().
 */
function indexpage_admin_settings() {
  drupal_add_css(drupal_get_path('module', 'indexpage') .'/indexpage.css');
  $yesno = array(1 => t('Yes'), 0 => t('No'));

  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    );

  $form['general']['indexpage_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description text for main index page'),
    '#default_value' => variable_get('indexpage_description', ''),
    '#cols' => 60,
    '#rows' => 3,
    );

  $form['general']['indexpage_maxresults'] = array(
    '#type' => 'textfield',
    '#title' => t('Max number of results per page'),
    '#default_value' => variable_get('indexpage_maxresults', 10),
    '#size' => 2,
    '#maxlength' => 2,
    '#description' => t('This option determines how many items will show on the results page before a pager is added. It also makes term list fieldsets collapse when there are more that this many terms in the list.'),
    );

  $form['general']['indexpage_show_users'] = array(
    '#type' => 'radios',
    '#options' => $yesno,
    '#prefix' => '<div class="indexpage-radios">',
    '#suffix' => '</div>',
    '#title' => t('Show users who have submitted this content'),
    '#default_value' => (int) variable_get('indexpage_show_users', FALSE),
    '#description' => t('If this option is selected, an additional section with user names will be shown.'),
    );

  $form['general']['indexpage_suppress_unused'] = array(
    '#type' => 'radios',
    '#options' => $yesno,
    '#prefix' => '<div class="indexpage-radios">',
    '#suffix' => '</div>',
    '#title' => t('Suppress unused taxonomy terms'),
    '#default_value' => (int) variable_get('indexpage_suppress_unused', FALSE),
    '#description' => t('If this option is selected, terms with no associated nodes will not be shown.'),
    );

  $form['general']['indexpage_teaser_tooltip'] = array(
    '#type' => 'radios',
    '#options' => $yesno,
    '#prefix' => '<div class="indexpage-radios">',
    '#suffix' => '</div>',
    '#title' => t("Use the node's teaser as the link title"),
    '#default_value' => (int) variable_get('indexpage_teaser_tooltip', TRUE),
    '#description' => t('If this option is selected, the teaser for the node will be used for the link title (tooltip).'),
    );

  $form['general']['indexpage_show_count'] = array(
    '#type' => 'radios',
    '#options' => $yesno,
    '#prefix' => '<div class="indexpage-radios">',
    '#suffix' => '</div>',
    '#title' => t('Show counts'),
    '#default_value' => (int) variable_get('indexpage_show_count', TRUE),
    '#description' => t('Shows a count of how many nodes there are and how many are used by each term.'),
    );

  $form['general']['indexpage_show_untagged'] = array(
    '#type' => 'radios',
    '#options' => $yesno,
    '#prefix' => '<div class="indexpage-radios">',
    '#suffix' => '</div>',
    '#title' => t('Show untagged'),
    '#default_value' => (int) variable_get('indexpage_show_untagged', TRUE),
    '#description' => t('Shows a count of how many nodes that have no taxonomy term associated.'),
    );

  $form['general']['indexpage_show_description'] = array(
    '#type' => 'radios',
    '#options' => $yesno,
    '#prefix' => '<div class="indexpage-radios">',
    '#suffix' => '</div>',
    '#title' => t('Show term description'),
    '#default_value' => (int) variable_get('indexpage_show_description', FALSE),
    '#description' => t('Shows the description for the term in the term list.'),
    );

  $form['general']['indexpage_list_format'] = array(
    '#type' => 'radios',
    '#options' => array('list' => t('List'), 'table' => t('Table')),
    '#title' => t('Show terms list as'),
    '#default_value' => variable_get('indexpage_list_format', 'list'),
    '#description' => t('This determines how the list of terms will appear.'),
    '#prefix' => '<div class="indexpage-radios">',
    '#suffix' => '</div>',
    );

  $field_list = array(
    'uid' => t('Author'),
    'authors' => t('All Authors'),
    'terms' => t('Terms'),
    'type' => t('Node type'),
    'created' => t('Created date/time'),
    'changed' => t('Updated date/time'),
    'status' => t('Published'),
    'promote' => t('Promoted'),
    'sticky' => t('Sticky'),
    'language' => t('Language'),
    );
  if (module_exists('weight')) {
    $field_list['weight'] = t('Weight');
  }

  $form['general']['indexpage_fields'] = array(
    '#type' => 'checkboxes',
    '#options' => $field_list,
    '#title' => t('Include extra fields on the result page'),
    '#default_value' => variable_get('indexpage_fields', array()),
    '#description' => t('Any selected fields will be included in the results list. The title will always be shown.'),
    '#prefix' => '<div class="indexpage-checkboxes">',
    '#suffix' => '</div>',
    );

  // Do a fieldset for each content type.
  $type_list = node_get_types('names');
  $taxonomy_exists = module_exists('taxonomy');

  foreach ($type_list as $type => $name) {
    $set = $type;
    $form[$set] = array(
      '#type' => 'fieldset',
      '#title' => t('Index page settings for !s', array('!s' => $type)),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      );

    $var_prefix = 'indexpage_'. $type;

    $form[$set][$var_prefix .'_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name to show for this node type'),
      '#default_value' => variable_get($var_prefix .'_name', $name),
      '#size' => 20,
      '#maxlength' => 50,
      );

    $form[$set][$var_prefix .'_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable index page for this node type'),
      '#return_value' => 1,
      '#default_value' => variable_get($var_prefix .'_enable', 1),
      );

    $form[$set][$var_prefix .'_alphaindex'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show alphabetical index for this node type'),
      '#return_value' => 1,
      '#default_value' => variable_get($var_prefix .'_alphaindex', 1),
      );

    if ($taxonomy_exists) {
      $form[$set][$var_prefix .'_vocfilter'] = array(
        '#type' => 'checkbox',
        '#title' => t('Show vocabulary filters for this node type'),
        '#return_value' => 1,
        '#default_value' => variable_get($var_prefix .'_vocfilter', 1),
        );
    }
    else {
      $form[$set][$var_prefix .'_vocfilter'] = array(
        '#type' => 'value',
        '#value' => FALSE,
        );
    }

    $form[$set][$var_prefix .'_page_text'] = array(
      '#type' => 'textarea',
      '#title' => t('Descriptive text to show at the top of the page.'),
      '#rows' => 3,
      '#default_value' => variable_get($var_prefix .'_page_text', NULL),
      );
  }

  $form['#submit'] = array('indexpage_settings_submit');

  return system_settings_form($form);
}

function indexpage_settings_submit($form, &$form_state) {
  menu_rebuild();
}
