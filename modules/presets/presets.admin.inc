<?php
/**
 * @file * Admin include file. 
*/

/**
 * Presets overview page.
 */
function presets_overview() {
  $item = menu_get_item('admin/presets');
  $content = system_admin_menu_block($item);

  $output = theme('admin_block_content', $content);

  return $output;
}

/** 
 * Displays the form for the standard settings tab.
 * 
 * @return array 
 * A structured array for use with Forms API. 
*/ 
function presets_page($preset) {
  $output = '';
  include_once './includes/install.inc';
  $presets = presets_get_presets();
  // Get current list of modules.
  $files = module_rebuild_cache();
  $dependencies = array();
  $download = array();
  $enable = array();
  $config = array();

  $output .= t('Refresh this page to see any changes.');
  $output .= '<h3>' . t('1. Modules') . '</h3>';  

  // Do the module checks.
  $settings = array();
  $settings['title'] = array(
    'title' => 'Modules',
    'value' => 'Name',
    'version' => 'Version',
    'action' => 'Action',
    'severity' => -1,
  );
  foreach($presets[$preset]['modules'] as $module => $info) {
    if (!$files[$module]) {
      $settings[$module] = array(
        'title' => $module,
        'value' => $info['name'],
        'version' => $info['version'],
        'action' => l('Download', 'http://drupal.org/project/' . (isset($info['project'])?$info['project']:$module), array('attributes' => array('target' => '_blank'))),
        'severity' => 2,
      );
    }
    else {
      if ($files[$module]->status == 0) {
        $settings[$module] = array(
          'title' => $module,
          'value' => $files[$module]->info['name'],
          'version' => $files[$module]->info['version'],
          'action' => l('Enable', 'admin/build/modules'),
          'severity' => 1,
        );
        // If this is the wrong version, bump the severity and make a note.
        if (isset($info['version']) && $files[$module]->info['version'] != $info['version']) {
          $settings[$module]['version'] .= ' (Recommended: ' . $info['version'] . ')';
          $settings[$module]['severity'] = 2;
        }
        foreach($files[$module]->info['dependencies'] as $dependency) {
          $dependencies[] = $dependency;
        }
      }
      elseif ($files[$module]->status == 1) {
        $settings[$module] = array(
          'title' => $module,
          'value' => $files[$module]->info['name'],
          'version' => $files[$module]->info['version'],
          'severity' => 0,
        );
        // If this is the wrong version, bump the severity and make a note.
        if (isset($info['version']) && $files[$module]->info['version'] != $info['version']) {
          $settings[$module]['version'] .= ' (Recommended: ' . $info['version'] . ')';
          $settings[$module]['severity'] = 1;
        }
        $config[$module] = $info;
      }
      if (!empty($info['submodules']) && is_array($info['submodules'])) {
        foreach($info['submodules'] as $submodule) {
          if ($files[$submodule]->status == 0) {
            $settings[$submodule] = array(
              'title' => $submodule,
              'value' => $files[$submodule]->info['name'],
              'version' => $settings[$module]['version'],
              'action' => l('Enable', 'admin/build/modules'),
              'severity' => 1,
            );
			if(is_array($files[$submodule]->info['dependencies'])) {
              foreach($files[$submodule]->info['dependencies'] as $dependency) {
                $dependencies[] = $dependency;
              }
			}
          }
        }
      }
    }
  }
  if (!empty($presets[$preset]['modules_footer'])) {
    $settings['footer'] = array(
      'value' => $presets[$preset]['modules_footer'],
      'severity' => -1,
    );
  }
  $output .= theme('presets_report', $settings); 
  $output .= "<br />\n";

  $output .= '<h3>' . t('2. Auto configure all') . '</h3>'; 
  $output .= t('Click "Reset All" to auto configure all modules to the presets.') . "<br />\n";
  $output .= l('Reset All', 'admin/presets/' . $preset . '/reset') . "<br />\n";
  $output .= "<br />\n";
  
  $output .= '<h3>' . t('3. Custom configurations') . '</h3>'; 
  
  // Do per module configs.
  foreach($config as $module => $info) {
    $settings = array();
    if ($info['settings']) {
      $settings['title'] = array(
        'title' => $info['name'],
        'value' => 'Setting',
        'action' => 'Action',
        'severity' => -1,
      );
      if ($presets[$preset]['file']) {
        include_once(drupal_get_path('module', $preset) . '/' . $presets[$preset]['file']);
      }
      foreach($info['settings'] as $setting) {
        $function = $preset . '_' . $module . '_' . $setting;
        if (function_exists($function)) {
          $settings[$setting] = $function();
          if ($settings[$setting]['severity'] != 0 && function_exists($function . '_reset')) {
            $settings[$setting]['action'] = l('Reset', 'admin/presets/settings/' . $preset . '/' . $module . '/' . $setting);
          }

        }
      }
      if (!empty($info['footer'])) {
        $settings['footer'] = array(
          'value' => $info['footer'],
          'severity' => -1,
        );
      }
      $output .= theme('presets_report', $settings);
    }
  }
  $output .= l('Reset all settings that can be reset.', 'admin/presets/' . $preset . '/reset');

  return $output;
}

function presets_disabled_modules() {
  $modules = array();
  $presets = presets_get_presets();
  $files = module_rebuild_cache();
  foreach($presets as $preset => $preset_info){
    foreach($preset_info['modules'] as $module => $info) {
      if ($files[$module] && $files[$module]->status == 0) {
        $dependencies_met = TRUE;
        foreach($files[$module]->info['dependencies'] as $dependency) {
          if (!$files[$dependency]) {
            $dependencies_met = FALSE;
          }
        }
        if ($dependencies_met) {
          $modules[$module] = $files[$module]->info['name'];
        }
      }
      if (!empty($info['submodules'])) {
        foreach($info['submodules'] as $submodule) {
          if ($files[$submodule] && $files[$submodule]->status == 0) {
            $dependencies_met = TRUE;
            foreach($files[$submodule]->info['dependencies'] as $dependency) {
              if (!$files[$dependency]) {
                $dependencies_met = FALSE;
              }
            }
            if ($dependencies_met) {
              $modules[$submodule] = $files[$submodule]->info['name'];
            }
          }
        }
      }
    }
  }
  return $modules;
}

/**
 * Generic preset reset page callback.
 */
function presets_reset_callback($preset, $module, $setting) {
  $presets = presets_get_presets();
  if ($presets[$preset]['file']) {
    include_once(drupal_get_path('module', $preset) . '/' . $presets[$preset]['file']);
  }
  $function = $preset . '_' . $module . '_' . $setting . '_reset';
  if (function_exists($function)) {
    $function();
  }
  else {
    drupal_set_message('Function: ' . $function  . '() not found');
  }
  drupal_goto('admin/presets/' . $preset);
}

/**
 * Form to reset all settings on a preset
 */
function presets_reset_all($preset, $redirect = TRUE) {
  $presets = presets_get_presets();
  if ($presets[$preset]['file']) {
    include_once(drupal_get_path('module', $preset) . '/' . $presets[$preset]['file']);
  }
  foreach($presets[$preset]['modules'] as $module => $info) {
    if (is_array($info['settings'])) {
      foreach($info['settings'] as $setting) {
        $check_function = $preset . '_' . $module . '_' . $setting;
        if (function_exists($check_function)) {
          $result = $check_function();
          if ($result['severity'] != REQUIREMENT_OK) {
            $function = $preset . '_' . $module . '_' . $setting . '_reset';
            if (function_exists($function)) {
              $function();
            }
          }
        }
      }
    }
  }
  if ($redirect) {
    drupal_goto('admin/presets/' . $preset);
  }
}

/**
 * Theme settings presets report.
 *
 * @param $requirements
 *   An array of requirements.
 * @ingroup themeable
 */
function theme_presets_report($requirements) {
  $i = 0;
  $output = '<table class="system-status-report">';
  foreach ($requirements as $requirement) {
    if (empty($requirement['#type'])) {
      $class = ++$i % 2 == 0 ? 'even' : 'odd';

      $classes = array(
        REQUIREMENT_INFO => 'info',
        REQUIREMENT_OK => 'ok',
        REQUIREMENT_WARNING => 'warning',
        REQUIREMENT_ERROR => 'error',
      );
      $class = $classes[isset($requirement['severity']) ? (int)$requirement['severity'] : 0] .' '. $class;

      // Output table row(s)
      if (!empty($requirement['description'])) {
        $output .= '<tr class="'. $class .' merge-down"><th>'. $requirement['title'] .'</th><td>'. $requirement['value'] .'</td><td>'. $requirement['version'] .'</td><td>'. $requirement['action'] .'</td></tr>';
        $output .= '<tr class="'. $class .' merge-up"><td colspan="3">'. $requirement['description'] .'</td></tr>';
      }
      else {
        if (!empty($requirement['title'])) {
          $output .= '<tr class="'. $class .'"><th>'. $requirement['title'] .'</th><td>'. $requirement['value'] .'</td><td>'. $requirement['version'] .'</td><td>'. $requirement['action'] .'</td></tr>';
        }
        else {
          $output .= '<tr class="'. $class .'"><td colspan="2">'. $requirement['value'] .'</td><td>'. $requirement['version'] .'</td><td>'. $requirement['action'] .'</td></tr>';
        }
      }
    }
  }

  $output .= '</table>';
  return $output;
}
