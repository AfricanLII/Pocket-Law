<?php 
// $Id:
/**
 * Install initial role profiles
 */
function tinybrowser_install_profiles() {
/*
  // already installed
  if (in_array(1, array_keys(variable_get('tinybrowser_profiles', array())))) {
    return FALSE;
  }
*/
  $profiles = array(
    1 => tinybrowser_admin_profile(), 
    2 => tinybrowser_sample_profile()
  );
  $index = 2;
  $role_profile = array();

  variable_set('tinybrowser_profiles', $profiles);
  variable_set('tinybrowser_roles_profiles', $role_profile);

  return $profiles;
}

/**
 * Admin profile
 */
function tinybrowser_admin_profile() {
  $profiles = variable_get('tinybrowser_profiles', array());
  if (isset($profile[1])) {
    return $profile[1];
  }
  $file_dir_path = base_path() . file_directory_path();
  $path_file  = $file_dir_path . '/';
  $path_image = $file_dir_path . '/images/';
  $path_media = $file_dir_path . '/media/';

  $profile = array(
    'name' => 'Admin',
    'max_file_size'  => 0,
    'max_image_size' => 0,
    'path_file'      => $path_file,
    'path_image'     => $path_image,
    'path_media'     => $path_media,
    'quota'          => 0,
    'permissions' => array(
      'upload'  => 'upload',
      'edit'    => 'edit',
      'folders' => 'folders',
      'delete'  => 'delete',
      'userpage' => 'userpage',
    ),
  );
  return $profile;
}

function tinybrowser_sample_profile() {
  $file_dir_path = base_path() . file_directory_path();
  $path_file  = $file_dir_path . '/%u/';
  $path_image = $file_dir_path . '/%u/images/';
  $path_media = $file_dir_path . '/%u/media/';

  $profile = array(
    'name' => 'Sample',
    'max_file_size'  => 1048576, // 1MB
    'max_image_size' => '640x480',
    'path_file'      => $path_file,
    'path_image'     => $path_image,
    'path_media'     => $path_media,
    'quota'          => 10485760, // 10MB
    'permissions' => array(
      'upload'   => 'upload',
      'edit'     => 'edit',
      'folders'  => 0,
      'delete'   => 'delete',
      'userpage' => 'userpage',
    ),
  );
  return $profile;
}
