<?php

/**
 * @file
 * Handles incoming requests to fire off regularly-scheduled tasks (cron jobs).
 */

include_once './includes/bootstrap.inc';
//echo theme('item_list', module_implements('cron'));exit;
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
drupal_cron_run();
