<?php
/**
 * @file helper to import fields provided by nodewords module (meta tags)
 */

/** 
 * Adds nodewords support.
 * cast keywords into the format that nodewords expects
 * 
 * Implementation  of HOOK_import_html()
 */
function nodewords_import_html($profile, &$node, $datadoc = NULL) {
  import_html_debug("Processing page to add nodewords metadata", array(), WATCHDOG_INFO);
  if ($node->keywords && ! $node->nodewords['keywords']) {
    $node->nodewords['keywords']['value'] = $node->keywords;
  }
  if ($node->description && ! $node->nodewords['description']) {
    $node->nodewords['description']['value'][] = $node->description;
  }

  // Nodewords causes errors if I set 'keywords' but don't set 'metatags'
  // Did its internal format change recently?
  if (isset($node->nodewords) && ! is_array($node->nodewords['metatags'])) {
    // Give it an empty array, 
    // else nodewords_save_tags($node->nodewords['metatags']) would throw errors.
    $node->nodewords['metatags'] = array();
  }
}