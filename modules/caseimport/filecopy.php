<?php
include 'file.inc';

function file_copy(&$source, $dest = 0, $replace = FILE_EXISTS_RENAME) {
  $dest = file_create_path($dest);

  $directory = $dest;
  $basename = file_check_path($directory);

  // Make sure we at least have a valid directory.
  if ($basename === FALSE) {
    $source = is_object($source) ? $source->filepath : $source;
    drupal_set_message(t('The selected file %file could not be uploaded, because the destination %directory is not properly configured.', array('%file' => $source, '%directory' => $dest)), 'error');
    watchdog('file system', 'The selected file %file could not be uploaded, because the destination %directory could not be found, or because its permissions do not allow the file to be written.', array('%file' => $source, '%directory' => $dest), WATCHDOG_ERROR);
    return 0;
  }

  // Process a file upload object.
  if (is_object($source)) {
    $file = $source;
    $source = $file->filepath;
    if (!$basename) {
      $basename = $file->filename;
    }
  }

  $source = realpath($source);
  if (!file_exists($source)) {
    drupal_set_message(t('The selected file %file could not be copied, because no file by that name exists. Please check that you supplied the correct filename.', array('%file' => $source)), 'error');
    return 0;
  }

  // If the destination file is not specified then use the filename of the source file.
  $basename = $basename ? $basename : basename($source);
  $dest = $directory . '/' . $basename;

  // Make sure source and destination filenames are not the same, makes no sense
  // to copy it if they are. In fact copying the file will most likely result in
  // a 0 byte file. Which is bad. Real bad.
  if ($source != realpath($dest)) {
    if (!$dest = file_destination($dest, $replace)) {
      drupal_set_message(t('The selected file %file could not be copied, because a file by that name already exists in the destination.', array('%file' => $source)), 'error');
      return FALSE;
    }

    if (!@copy($source, $dest)) {
      drupal_set_message(t('The selected file %file could not be copied.', array('%file' => $source)), 'error');
      return 0;
    }

    // Give everyone read access so that FTP'd users or
    // non-webserver users can see/read these files,
    // and give group write permissions so group members
    // can alter files uploaded by the webserver.
    @chmod($dest, 0664);
  }

  if (isset($file) && is_object($file)) {
    $file->filename = $basename;
    $file->filepath = $dest;
    $source = $file;
  }
  else {
    $source = $dest;
  }

  return 1; // Everything went ok.
}
?>
