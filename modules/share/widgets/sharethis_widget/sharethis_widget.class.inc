<?php

class sharethis_widget extends share {
  var $module = 'sharethis_widget';

  function visible($node, $teaser, $page = TRUE) {
    return (user_access('use sharethis widget') && user_access('use share this') && parent::visible($node, $teaser, $page));
  }

  function render($node) {
    return theme('sharethis', $node->title, drupal_get_path_alias('node/'. $node->nid));
  }
}
