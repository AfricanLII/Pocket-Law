<?php
/**
 * hook_presets();
 *
 * hook_presets is the basic hook for implementing the presets module.  It is where
 * the main definition for your presets is created.  It should follow the structure
 * of having one or more root items which define individual presets.
 *
 */
function hook_presets() {
  return array(
    'seotools' => array( // This defines the preset (you can have multuple presets in one hook file)
      'title' => 'SEO Tools', // this is the title that will be displayed on the admin/presets/seotools page
      'version' => 1, // Set the version for the presets.  Enforces that this is supposed to be a preset.
      'description' => 'Presets for SEO optimization',
      'file' => 'plugins/seotools.presets.inc', // If the preset definition or callback functions are stored in another file.
      'modules_footer' => l('Download all modules', 'http://leveltendesign.com') . ' from LevelTen Interactive',
      'modules' => array( // These are modules that will be required for this preset.
        'seotools' => array( // The project name from drupal.org for this module.
          'name' => 'SEO Tools',
          'project' => 'seotools', // (optional) the drupal.org project name.  Defaults to module name.
          'footer' => 'To change the settings, go to ' . l('admin/settings/seotools', 'admin/settings/seotools'),
          'settings' => array( // Settings will be function callbacks for each setting.  The function name will be {preset}_{module}_{setting}();
            'seotools_uid'
          ),
        ),
        'wordstream' => array(
          'name' => 'Wordstream',
          'version' => '6.x-1.0-alpha1', // You can specify a recommended version.
          'footer' => 'To change the settings, go to ' . l('admin/settings/wordstream', 'admin/settings/wordstream'),
          'settings' => array(
            'wordstream_username',
            'wordstream_free_embed_id',
          ),
        ),
        'xmlsitemap' => array(
          'name' => 'XML Sitemap',
          'version' => '6.x-2.0-beta1',
          'submodules' => array( // You can specify submodules which are contained in the main module that should be enabled as well.
            'xmlsitemap_custom',
            'xmlsitemap_node',
            'xmlsitemap_engines',
            'xmlsitemap_user',
            'xmlsitemap_menu',
            'xmlsitemap_taxonomy',
           ),
        ),
      ),
    ),
  );
}

/**
 * Settings callback
 *
 * Each of the settings for the modules will have a callback function that returns an array
 * similar to hook_requirements.
 *
 * It is named based on the name of the preset, module and setting.  The function name should be
 * {preset}_{module}_{setting}().
 *
 * return array(
 *   'title' => 'Title',
 *   'value' => 'Value',
 *   'description' => 'Description',
 *   'severity' => REQUIREMENT_ERROR,
 * );
 *
 * Severity can be:
 * define('REQUIREMENT_INFO', -1);
 * define('REQUIREMENT_OK', 0);
 * define('REQUIREMENT_WARNING', 1);
 * define('REQUIREMENT_ERROR', 2);
 */
// Settings callback for {seotools} preset, {wordstream} module and {wordstream_free_embed_id} setting.
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

/**
 * Settings reset function
 *
 * You can also define a function that has the same name as the settings callback but includes _reset at the end.
 * If this function exists, a "Reset" action appears on the settings row and when it is clicked, the function is
 * called.
 *
 */
// Reset function for {seotools} preset, {page_title} module and {nodes} setting.
function seotools_page_title_nodes_reset() {
  $types = node_get_types();
  foreach ($types as $type) {
    // Define the node-type key
    $key = 'page_title_type_'. $type->type . '_showfield';
    variable_set($key, 1);
  }
}

?>
