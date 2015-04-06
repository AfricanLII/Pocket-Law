<?php
/**
 * @file
 * Enables taxonomy terms to be exported and imported.
 *
 * When exporting, all if the vocabularies details will be exported
 * along with all the terms.
 *
 * When importing, all of the vocabularies on the receiving instance
 * will be checked first to check that they match the vocabularies
 * in the dataset. If there are any differences in the vocabularies
 * then they import routine will raise an error then abort.
 *
 * After updating or inserting a taxonomy term there will be a check
 * to see if the i18ntaxonomy module is installed.  If it is installed
 * then the value of the 'language' column of the {term_data} table
 * will be updated with the imported value.
 *
 * It is better that this is called after the term is updated or
 * inserted with the API functions as the hook_taxonomy() function of
 * i18ntaxonomy.module incorrectly sets the value to a default value
 * if it can't find a value in a couple of forms.
 */

/**
 * Function to add items to the main menu.
 */
function data_export_import_taxonomy_terms_add_items_to_menu(&$items) {
  // Here we need to loop through the profile files and add in items as
  // required.  This would be from a function
  // data_export_import_profile_add_items_to_main_form(&$items)
  // sort of array.  First of all the taxonomy terms.
  $items['admin/content/data_export_import/taxonomy_terms'] = array(
    'title' => 'Taxonomy Terms',
    'page callback' => 'data_export_import_callback_export_taxonomy_terms',
    'access arguments' => array('access data export import'),
    'weight' => 1,
    'description' => 'Export Taxonomy Terms',
    'type' => MENU_LOCAL_TASK,
  );

  $items['admin/content/data_export_import/taxonomy_terms/export'] = array(
    'title' => 'Export',
    'page callback' => 'data_export_import_callback_export_taxonomy_terms',
    'access arguments' => array('access data export import'),
    'weight' => 2,
    'description' => 'Export Taxonomy Terms',
    'type' => MENU_DEFAULT_LOCAL_TASK,
  );

  $items['admin/content/data_export_import/taxonomy_terms/import'] = array(
    'title' => 'Import',
    'page callback' => 'data_export_import_callback_import_taxonomy_terms',
    'access arguments' => array('access data export import'),
    'weight' => 3,
    'description' => 'Import Taxonomy Terms',
    'type' => MENU_LOCAL_TASK,
  );
}

/**
 * Callback function to export taxonomy terms.
 */
function data_export_import_callback_export_taxonomy_terms() {

  return drupal_get_form('data_export_import_export_taxonomy_terms_form');
}

/**
 * Callback function to import taxonomy terms.
 */
function data_export_import_callback_import_taxonomy_terms() {

  return drupal_get_form('data_export_import_import_taxonomy_terms_form');
}

/**
 * Function to create form to export taxonomy terms.
 */
function data_export_import_export_taxonomy_terms_form($form_state) {

  $form = array();

  // This stops the values being posted from being flattened into a
  // single level array.  It might be easier to extract the
  // information from a non-flattened array.
  $form['#tree'] = TRUE;

  data_export_import_taxonomy_terms_add_fieldset_to_export_form($form);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create dataset files'),
  );

  return $form;
}

/**
 * Function to create form to import taxonomy terms.
 */
function data_export_import_import_taxonomy_terms_form($form_state) {

  $form = array();

  $form['taxonomy_terms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy terms'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // Get the contents of the dataset directory and create a list of
  // links to dataset files.
  $directory = file_directory_path() . "/data_export_import/taxonomy_terms";
  $mask = '.dataset';
  $files = file_scan_directory($directory, $mask);

  // Sort them by the filename which is used as the key.  Since the
  // files are named using datetime stamps they will be listed in
  // date/time order.
  ksort($files);

  $options = array();
  $options['none'] = "None";
  foreach ($files as $file) {

    $options[$file->basename] = check_plain($file->basename);
  }

  $form['taxonomy_terms']['taxonomy_terms'] = array(
    '#type' => 'radios',
    '#title' => t('Please select file to import'),
    '#default_value' => 'none',
    '#options' => $options,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Import dataset files',
  );

  return $form;
}

/**
 * Add fieldset to the data export form.
 */
function data_export_import_taxonomy_terms_add_fieldset_to_export_form(&$form) {

  // Taxonomy terms.
  $form['taxonomy_terms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy terms'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,);

  $form['taxonomy_terms']['taxonomy_terms'] = array(
    '#type' => 'checkbox',
    '#title' => 'Export all Taxonomy terms',);
}

/**
 * Function to process form to export taxonomy terms.
 */
function data_export_import_export_taxonomy_terms_form_submit($form, &$form_state) {

  // This module has not yet been extended to correctly handle data
  // which has internationalization (i18n) enabled.
  if (module_exists('i18n')) {

    drupal_set_message(t("The data export functionality is not currently compatible with internationalization (i18n)."), 'error');
    return TRUE;
  }

  data_export_import_taxonomy_terms_export_to_file($form_state, $dataset_files_created);

  drupal_set_message(t("The following dataset files were created:"));

  foreach ($dataset_files_created as $dataset_file_name) {
    drupal_set_message(check_plain($dataset_file_name));
  }

  return TRUE;
}

/**
 * Function to process form to import taxonomy terms.
 */
function data_export_import_import_taxonomy_terms_form_submit($form, &$form_state) {

  // This module has not yet been extended to correctly handle data
  // which has internationalization (i18n) enabled.
  if (module_exists('i18n')) {

    drupal_set_message(t("The data export functionality is not currently compatible with internationalization (i18n)."), 'error');
    return TRUE;
  }

  if ($form_state['values']['taxonomy_terms'] != 'none') {

    $result = data_export_import_import_taxonomy_terms($form_state['clicked_button']['#post']['taxonomy_terms']);

    if ($result) {
      drupal_set_message(t('The Taxonomy Terms dataset file %dataset_file was imported.', array('%dataset_file' => $form_state['clicked_button']['#post']['taxonomy_terms'])));
    }
    else {
      drupal_set_message(t('The Taxonomy Terms dataset file %dataset_file failed to import.', array('%dataset_file' => $form_state['clicked_button']['#post']['taxonomy_terms'])), 'error');

    }
  }
}

/**
 * Export the required dataset file.
 */
function data_export_import_taxonomy_terms_export_to_file($form_state, &$dataset_files_created) {

  if ($form_state['clicked_button']['#post']['taxonomy_terms']['taxonomy_terms'] == 1) {
    $dataset_files_created[] = data_export_import_export_taxonomy_terms();
  }
}

/**
 * Exports detailed data about the vocabularies and its terms.
 *
 * This function is going to use the API to extract the relevant
 * data. This data is then going to be output to a data file.
 *
 * The first element in the array will be an array of the
 * vocabularies and the vocabulary details. This will be used
 * during the import to check that there are the correct
 * vocabularies in place which are ready to receive the terms being
 * imported.
 *
 * The second element will contain details of all the terms which
 * are in each vocabulary. These terms will be used to ensure that
 * the receiving instance ends up with a matching set of terms.
 *
 * @return string
 *   The filename of the datafile which was created.
 */
function data_export_import_export_taxonomy_terms() {

  // This will be the main array which will hold the data which will
  // be output to the dataset file.
  $dataset = array();

  // Add the vocabularies data the the array which will exported.
  $vocabularies_array = taxonomy_get_vocabularies();
  $dataset['vocabularies'] = $vocabularies_array;

  // Add the vocabulary terms to the main array.
  // We will loop through the vocabularies array and use the vid to
  // then get the list of terms in each vocabulary.
  foreach ($vocabularies_array as $vocabulary) {

    $vocabulary_id = $vocabulary->vid;

    // Here we will get the terms related to the vocabulary - then
    // we'll add on the synonyms and add on the related terms - then
    // we'll add the array on terms to the main array.
    $terms_array = data_export_import_taxonomy_get_tree_with_reset($vocabulary->vid, 0, -1, NULL, TRUE);

    // The $list_of_terms will be used to dedupe the terms returned by
    // data_export_import_taxonomy_get_tree_with_reset() which outputs
    // terms twice if they have two parents.
    $list_of_terms = array();
    foreach ($terms_array as $array_key => $term) {

      // If the term has been exported already then we don't want to
      // export it again. So we'll remove the term from the array.
      if (in_array($term->tid, $list_of_terms)) {
        unset($terms_array[$array_key]);
      }
      else {

        // Here we are going to add all the term id to an array to
        // keep a list of all terms currently output. This is because
        // when terms have more than one parent they are output twice.
        // What we are going to do is to check to see if the term has
        // been output already - and if so then not re-output it
        // again.
        $list_of_terms[] = $term->tid;

        // Here we add the synonyms to the term object being
        // exported.
        $synonyms_array = taxonomy_get_synonyms($term->tid);

        // Sort the array to make it consistent with a later call to
        // this function. When they are extracted using the API
        // function  they are not sorted in any particular order.
        asort($synonyms_array);

        $term->synonyms = $synonyms_array;

        // Add in the related terms.
        $related_terms = taxonomy_get_related($term->tid);

        $array_of_related_term_tids = array();

        foreach ($related_terms as $related_term) {
          $array_of_related_term_tids[] = $related_term->tid;
        }

        $term->relations = $array_of_related_term_tids;
      }
    }

    // Before we add the terms to the data being output we are going
    // to set the array key value to be the term id. Currently the
    // array of terms have a sequential key: 0,1,2,3 etc - these will
    // be changed to be the term id value. As the terms id's are
    // unique then there will be no conflict - and this makes the
    // array more useful later on.
    $terms_array_with_tid_as_key = array();
    foreach ($terms_array as $term) {
      $terms_array_with_tid_as_key[$term->tid] = $term;
    }

    // Now add the array of terms to the dataset array and use the vid
    // as the key. Using the vid as key makes it easier to search this
    // array during the import.
    $dataset['vocabulary_terms'][$vocabulary_id] = $terms_array_with_tid_as_key;
  }

  // Here we will serialize the array to convert it to a string which
  // can then be output to a file.
  $dataset_serialized = serialize($dataset);

  // Create the default directory to hold the datasets.
  $dataset_directory_parent_directory = file_directory_path() . "/data_export_import";
  file_check_directory($dataset_directory_parent_directory, $mode = FILE_CREATE_DIRECTORY);

  $dataset_directory = file_directory_path() . "/data_export_import/taxonomy_terms/";
  file_check_directory($dataset_directory, $mode = FILE_CREATE_DIRECTORY);

  // Save the string as a file. This is the dataset data file.
  $file_name = format_date(time(), 'custom', 'Ymd_His') . "_taxonomy_terms.dataset";
  $file_path_and_name = $dataset_directory . "/" . $file_name;

  file_save_data($dataset_serialized, $file_path_and_name, FILE_EXISTS_REPLACE);

  return $file_name;
}

/**
 * Make taxonomy terms match the terms from a dataset file.
 *
 * This function is to effectively 'import' the vocabularies and all
 * the terms in those vocabularies.
 *
 * As we are expecting the vocabularies to exist already then we are
 * not going to import them but check that they exist and are the same
 * as in the imported dataset.
 *
 * NB - Later we may extend this to create any new vocabularies if
 * necessary.
 *
 * Then we are going to bring in the terms from the dataset file.
 *
 * NB - When this import has finished the terms in the receiving
 * instance should be an exact match with the terms in the imported
 * dataset file.  Think in terms of rsync with the '--delete'
 * option. This means that as well as importing new terms and updating
 * existing terms we need to delete terms from the reciving instance
 * which are not in the imported dataset.
 *
 * This synchronisation will be carried out by two passes.
 *
 * First we will loop through the terms in the receiving instance  and
 * check against the the imported dataset. If the term exists in the
 * dataset then it will be updated in the receiving instance.  If it
 * doesn't exist in the dataset then it will be deleted from the
 * receiving Drupal instance.
 *
 * The second pass will be a loop through the dataset - any terms
 * which are in the dataset but are not in the receiving Drupal
 * instance will be created.
 *
 * This will effectively mean that the terms have been sychronised
 * completely.
 *
 * NB - There is some deeper logic here we need to be aware of.  The
 * term ID's need to match exactly otherwise the related terms will
 * not match.  So when the new terms are created they will need to
 * have their old ID's set to match exactly - again due to the related
 * terms.
 *
 * Also, this should deal with the scenario where a new term has been
 * created in the receiving instance which has the same tid as one
 * being imported but it is in a different vocabulary.
 *
 * @param string $file
 *   The dataset file which is being imported.
 *
 * @return bool
 *   TRUE if the import process ran without errors.
 */
function data_export_import_import_taxonomy_terms($file) {

  // @todo Need to add a check that the file exists and handle the error.
  // *
  // Load the dataset file into a variable.
  $file_content = file_get_contents(file_directory_path() . "/data_export_import/taxonomy_terms/" . $file);

  // Decode the serialized data and store it in an array of objects.
  $file_content_as_array = unserialize($file_content);

  // This will check that the vocabularies in the dataset file all
  // exist in the receiving instance and are an exact match.
  // NB - The receiving instance may have extra vocabularies which
  // have been created.  This is OK because even though there is not
  // an exact match at least the vocabularies which are needed to
  // enable the terms to be imported all exist and are an exact match
  // to the vocabularies exported from.
  foreach ($file_content_as_array['vocabularies'] as $vocabulary_from_file) {

    // Get the existing vocabulary as an object.
    // Second parameter is set to TRUE to reset the cache - needed as
    // we call this function after we've unset some values in a
    // previous usage.
    $existing_vocabulary = taxonomy_vocabulary_load($vocabulary_from_file->vid, TRUE);

    // Here we need to carry out a little fix to make the formats
    // match - this probably should not be needed and has been
    // reported as http://drupal.org/node/1286320.
    unset($vocabulary_from_file->type);

    // Here we need to unset the hierarchy value for both objects.
    // This is for two reasons.
    // 1. The hierarchy value is different dependent on the terms.
    // I.e. If any term has two parents then it should be set to a
    // value of '2'.  So we can't expect the values to be the same
    // because the imported terms may have different numbers of
    // parents etc.
    // 2. The hierarchy value does not appear to be being set properly
    // all the time - this has been registered as a bug at:
    // http://drupal.org/node/1309990.
    unset($vocabulary_from_file->hierarchy);
    unset($existing_vocabulary->hierarchy);

    // We will also unset the nodes properties.  The receiving instance may
    // have different content types - and as part of a new setup the
    // vocabularies may be linked to different content types.
    unset($vocabulary_from_file->nodes);
    unset($existing_vocabulary->nodes);


    // Check the vocabularies actually match.
    if ($vocabulary_from_file != $existing_vocabulary) {

      drupal_set_message(t('There is a mismatch between the vocabularies in the dataset file and the currentvocabularies - vocabulary name: %vocabular_name', array('%vocabular_name' => $vocabulary_from_file->name)), 'error');

      return FALSE;
    }
  }

  // Here we will loop through the vocabulary terms in the receiving
  // instance.  For each term we will look into the dataset being
  // imported.  If the term exists in the imported dataset then we
  // will update it - if it does not exist then we will delete it.
  // *
  // We will use the API to get the list of vocabularies - and then
  // loop through each vocabulary and get all the terms. This should
  // deal with the issue where a new term has been created in the
  // receiving instance which has the same tid as one being imported
  // but is in a different vocabulary.
  // *
  // NB - Effectively all new terms in the receiving instance will be
  // deleted - which is fine as we don't want them.
  // *
  // In this case we will first need to delete this term completely in
  // the receiving instance.  It will then be created again when we
  // loop through the dataset as it will be created and assigned to
  // the correct vocabulary.
  // *
  // This should mean that the terms for a vocabulary in the receiving
  // instance match exactly the terms in the imported dataset.
  $existing_vocabularies_array = taxonomy_get_vocabularies();

  // We will loop through the vocabularies array and use the vid to
  // then get the list of terms in that vocabulary.
  foreach ($existing_vocabularies_array as $existing_vocabulary) {

    // Get the vocabulary ID so we can use it to get the terms in that
    // vocabulary.
    $existing_vocabulary_id = $existing_vocabulary->vid;

    // Here we are looping through all of the terms in the vocabulary.
    // NB - These are terms which are in the receiving instance and we
    // are going to check them against the terms in the dataset being
    // imported.
    $existing_terms_array = data_export_import_taxonomy_get_tree_with_reset($existing_vocabulary_id, 0, -1, NULL, TRUE);

    // We will use this array to hold all the terms id's which have
    // been found.  The function taxonomy_get_tree_with_reset outputs
    // terms more than once if they have multiple parents. This will
    // mean we can ignore extra copies of the terms.
    $list_of_terms = array();
    foreach ($existing_terms_array as $existing_term) {

      // If the term has been exported already then we don't want to
      // export it again so we'll go to the next term in the foreach loop.
      if (in_array($existing_term->tid, $list_of_terms)) {

        continue;
      }

      // Add the term id to the list we've worked on.
      $list_of_terms[] = $existing_term->tid;

      // Look for a term in the dataset being imported which matches
      // the existing term.
      // *
      // If one doesn't exist then we'll delete it from the existing
      // vocabulary.
      // *
      // If we find a matching term then we will check if the existing
      // term is an exact match or not - and if it is not then we'll
      // update the existing term with the term data from the dataset.
      // *
      // Check to see if the term can be found in the dataset file
      // with a matching vocabulary id and term id. If it does not
      // exist then is should be deleted from the receiving instance.
      if (!isset($file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid])) {

        $return_status = taxonomy_del_term($existing_term->tid);
      }

      if (isset($file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid])) {

        // We are going to check that the terms match exactly.  First
        // we need to add some data to the existing terms and unset
        // some fields.
        // *
        // The the existing term does not match the term in the
        // dataset exactly then it will be updated to make it match
        // the term being imported.
        // *
        // We will sort the synonyms to put them in the same order as
        // they are in the dataset file.
        $synonyms_array = taxonomy_get_synonyms($existing_term->tid);
        asort($synonyms_array);
        $existing_term->synonyms = $synonyms_array;

        // Here we will add in the related terms to this object - this
        // means we can do a full check if the term objects.
        $related_terms = taxonomy_get_related($existing_term->tid);

        $array_of_related_term_tids = array();
        foreach ($related_terms as $related_term) {
          $array_of_related_term_tids[] = $related_term->tid;
        }
        $existing_term->relations = $array_of_related_term_tids;

        // We unset the depth as this is calculated and can be
        // different when terms have two parents which are at
        // different depths.
        unset($existing_term->depth);
        unset($file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid]->depth);

        // If the terms don't match then update the existing term.
        // *
        // Get the object which is the existing term in the receiving
        // instance.
        $existing_term_object = $existing_term;

        // Get the term object from the dataset array.
        $term_object_from_dataset = $file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid];

        if ($existing_term_object == $term_object_from_dataset) {

          // Don't update the term - maybe carry out a count later.
          $place_holder = 1;
        }

        // Update the term with the data from the dataset.
        if ($existing_term_object != $term_object_from_dataset) {

          // The synonyms need to be converted to a list separated by
          // new lines before we can use it.
          $synonym_list = "";
          foreach ($file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid]->synonyms as $synonym) {
            $synonym_list .= $synonym . "\n";
          }

          // Update the term with data from the dataset.
          $term_data_array = array(
            'tid' => $existing_term->tid,
            'vid' => $existing_vocabulary_id,
            'name' => $file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid]->name,
            'description' => $file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid]->description,
            'weight' => $file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid]->weight,
            'parent' => $file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid]->parents,
            'synonyms' => $synonym_list,
            'relations' => $file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid]->relations,
          );

          // Here we are using the standard taxonomy_save_term() API
          // function as it handles terms being updated OK.
          $save_term_status = taxonomy_save_term($term_data_array);
          if ($save_term_status != 2) {
            drupal_set_message(t("Term not saved correctly.  Term id:") . $existing_term->tid . "\n", 'error');
          }

          // Here we need to update the language field if the translation
          // module has been enabled.  This also picks up the fault of
          // the hook_taxonomy() function in i18ntaxonomy.module from
          // setting the language to a default setting if it can't find
          // a value in a couple of forms.  hook_taxonomy() is called
          // when using the
          // data_export_import_taxonomy_save_term_with_defined_tid()
          // function.
          if (module_exists('i18ntaxonomy')) {

            $query_result = db_query("UPDATE {term_data} SET language = '%s' WHERE tid = %d", $file_content_as_array['vocabulary_terms'][$existing_vocabulary_id][$existing_term->tid]->language, $existing_term->tid);
            if ($query_result == FALSE) {
              drupal_set_message(t("Insertion into term_data table failed."), 'error');
            }
          }

        }
      }
    }
  }

  // Here we will pick up if the file contains no taxonomy terms.
  // if (count($file_content_as_array['vocabulary_terms']) == 0) {
  if (empty($file_content_as_array['vocabulary_terms'])) {
    return TRUE;
  }

  // Loop through the terms in the dataset being imported and if they
  // don't exist in the receiving instance then we will create them.
  foreach ($file_content_as_array['vocabulary_terms'] as $vid => $vocabulary_from_dataset) {

    foreach ($vocabulary_from_dataset as $term_from_dataset) {

      $get_term_result = taxonomy_get_term($term_from_dataset->tid, $reset = TRUE);

      if (!$get_term_result) {

        // Will use the a modified taxonomy_save_term API function to
        // create the term. The synonyms need to be converted to a
        // list separated by new lines before we can use it.
        $synonym_list = "";
        foreach ($term_from_dataset->synonyms as $synonym) {
          $synonym_list .= $synonym . "\n";
        }

        // Update the term with data from the dataset.
        $term_data_array = array(
          'tid' => $term_from_dataset->tid,
          'vid' => $term_from_dataset->vid,
          'name' => $term_from_dataset->name,
          'description' => $term_from_dataset->description,
          'weight' => $term_from_dataset->weight,
          'parent' => $term_from_dataset->parents,
          'synonyms' => $synonym_list,
          'relations' => $term_from_dataset->relations,
        );

        $save_term_status = data_export_import_taxonomy_save_term_with_defined_tid($term_data_array);

        if ($save_term_status != 1) {
          drupal_set_message(t("Term not saved correctly.  Term id:") . $existing_term->tid . "\n", 'error');
        }

        // Here we need to update the language field if the translation
        // module has been enabled.  This also picks up the fault of
        // the hook_taxonomy() function in i18ntaxonomy.module from
        // setting the language to a default setting if it can't find
        // a value in a couple of forms.  hook_taxonomy() is called
        // when using the
        // data_export_import_taxonomy_save_term_with_defined_tid()
        // function.
        if (module_exists('i18ntaxonomy')) {
          $query_result = db_query("UPDATE {term_data} SET language = '%s' WHERE tid = %d", $term_from_dataset->language, $term_from_dataset->tid);
          if ($query_result == FALSE) {
            drupal_set_message(t("Insertion into term_data table failed."), 'error');
          }
        }
      }
    }
  }

  // Loop through all the vocabularies and reset the hierarchy
  // value. This value is a calculated value which is stored in the
  // vocabularies table and will need to be calculated dependent on
  // the structure of the terms in the vocabulary.
  // Value will be either:
  // 0 - Flat list of terms.
  // 1 - One or more terms have a parent.
  // 2 - One or more terms have more than one parent.
  $vocabularies_array = taxonomy_get_vocabularies();
  foreach ($vocabularies_array as $vocabulary) {
    data_export_import_reset_taxonomy_vocabulary_hierarchy($vocabulary->vid);
  }

  return TRUE;

}

/**
 * This resets the value of the hierarchy field for the vocabulary.
 *
 * The value of the hierarchy field in the vocabulary is used to set
 * up how the admin screen works when editing terms. I.e. If the value
 * is set to '2' this means that there are terms with multiple parents
 * and consequently the drag and drop interface pn the admin page will
 * be disabled.
 *
 * As the terms are being programmatically updated by this module the
 * hierarchy value my not be correct for the terms and so needs to be
 * reset.
 *
 * @param int $vid
 *   Vocabulary ID of the vocabulary having it's hierarchy reset.
 *
 * @return bool
 *   TRUE if the function ran without error.
 */
function data_export_import_reset_taxonomy_vocabulary_hierarchy($vid) {

  // Get the tree of terms we are going to examine.
  $tree = data_export_import_taxonomy_get_tree_with_reset($vid, 0, -1, NULL, TRUE);

  // Set the hierarchy to the lowest level and increase it if relevant
  // terms are found.
  $hierarchy = '0';
  foreach ($tree as $term) {

    // Check this term's parent count.
    if (count($term->parents) > 1) {

      $hierarchy = '2';

      // Break out the foreach loop as the highest hierarchy value has
      // been set.
      break;
    }
    // Look for a single parent - but do not set the value if there is
    // a single parent value of '0' which is normal for terms with no
    // parents.
    elseif (count($term->parents) == 1 && '0' !== array_shift($term->parents)) {
      $hierarchy = '1';
    }
  }

  // Second parameter is set to TRUE to reset the cache.
  $vocabulary_object = taxonomy_vocabulary_load($vid, TRUE);

  // If the hieracrchy value has changed then save the new value back
  // to the database.
  if ($hierarchy != $vocabulary_object->hierarchy) {

    $vocabulary_object->hierarchy = $hierarchy;

    // Needs to be cast to an array as that is what the
    // taxonomy_save_vocabulary function expects.
    $vocabulary_array = (array) $vocabulary_object;
    taxonomy_save_vocabulary($vocabulary_array);
  }
}

/**
 * This function overrides the taxonomy_get_tree() API function.
 *
 * The extra functionality is to provide a reset flag as a
 * parameter. This enables the static attributes to be unset. This is
 * important as this function is used multiple times during the course
 * of this module. If it was not possible to unset the statics the
 * values would stay the same between the function calls resulting in
 * stale data being returned.
 *
 * @param int $vid
 *   Which vocabulary to generate the tree for.
 *
 * @param int $parent
 *   The term ID under which to generate the tree. If 0, generate the
 *   tree for the entire vocabulary.
 *
 * @param int $depth
 *   Internal use only.
 *
 * @param int $max_depth
 *   The number of levels of the tree to return. Leave NULL to return
 *   all levels.
 *
 * @param bool $reset
 *   Whether or not to clear the static variables to force this
 *   function to get fresh data
 *
 * @return array
 *   An array of all term objects in the tree. Each term object is
 *   extended to  have "depth" and "parents" attributes in addition to
 *   its normal ones. Results are statically cached.
 */
function data_export_import_taxonomy_get_tree_with_reset($vid, $parent = 0, $depth = -1, $max_depth = NULL, $reset = FALSE) {
  static $children, $parents, $terms;

  if ($reset) {
    unset($children[$vid]);
    unset($parents[$vid]);
    unset($terms[$vid]);
  }

  $depth++;

  // We cache trees, so it's not CPU-intensive to call get_tree() on a
  // term and its children, too.
  if (!isset($children[$vid])) {
    $children[$vid] = array();

    $result = db_query(db_rewrite_sql('SELECT t.tid, t.*, parent FROM {term_data} t INNER JOIN {term_hierarchy} h ON t.tid = h.tid WHERE t.vid = %d ORDER BY weight, name', 't', 'tid'), $vid);
    while ($term = db_fetch_object($result)) {
      $children[$vid][$term->parent][] = $term->tid;
      $parents[$vid][$term->tid][] = $term->parent;
      $terms[$vid][$term->tid] = $term;
    }
  }

  $max_depth = (is_null($max_depth)) ? count($children[$vid]) : $max_depth;
  $tree = array();
  if ($max_depth > $depth && !empty($children[$vid][$parent])) {
    foreach ($children[$vid][$parent] as $child) {
      $term = drupal_clone($terms[$vid][$child]);
      $term->depth = $depth;

      // The "parent" attribute is not useful, as it would show one
      // parent only.
      unset($term->parent);
      $term->parents = $parents[$vid][$child];
      $tree[] = $term;
      if (!empty($children[$vid][$child])) {
        $tree = array_merge($tree, data_export_import_taxonomy_get_tree_with_reset($vid, $child, $depth, $max_depth));
      }
    }
  }
  return $tree;
}

/**
 * This function is a modified version of the taxonomy_save_term function.
 *
 * @param array &$form_values
 *   Array of term data which is to be saved.
 *
 * @return int
 *   $status value showing if the term was saved OK.
 */
function data_export_import_taxonomy_save_term_with_defined_tid(&$form_values) {

  // I think this has the effect of making sure these values exist on
  // the array but not overwrite any existing values.
  $form_values += array(
    'description' => '',
    'weight' => 0,
  );

  // Insert the record into the term_data table.
  // We're using a direct insertion as the drupal_write_record
  // function does not allow us to insert a new row with a defined id
  // value.
  $query_result = db_query("INSERT INTO {term_data} (`tid`, `vid`, `name`, `description`, `weight`) VALUES (%d, %d, '%s', '%s', %d)", $form_values['tid'], $form_values['vid'], $form_values['name'], $form_values['description'], $form_values['weight']);
  if ($query_result == FALSE) {
    drupal_set_message(t("Insertion into term_data table failed."));
  }

  // Set up parameters used later when calling hook_taxonomy().
  $hook = 'insert';
  $status = SAVED_NEW;

  // Deal with the data in the related tables.
  db_query('DELETE FROM {term_relation} WHERE tid1 = %d OR tid2 = %d', $form_values['tid'], $form_values['tid']);
  if (!empty($form_values['relations'])) {
    foreach ($form_values['relations'] as $related_id) {
      if ($related_id != 0) {
        db_query('INSERT INTO {term_relation} (tid1, tid2) VALUES (%d, %d)', $form_values['tid'], $related_id);
      }
    }
  }

  db_query('DELETE FROM {term_hierarchy} WHERE tid = %d', $form_values['tid']);
  if (!isset($form_values['parent']) || empty($form_values['parent'])) {
    $form_values['parent'] = array(0);
  }
  if (is_array($form_values['parent'])) {
    foreach ($form_values['parent'] as $parent) {
      if (is_array($parent)) {
        foreach ($parent as $tid) {
          db_query('INSERT INTO {term_hierarchy} (tid, parent) VALUES (%d, %d)', $form_values['tid'], $tid);
        }
      }
      else {
        db_query('INSERT INTO {term_hierarchy} (tid, parent) VALUES (%d, %d)', $form_values['tid'], $parent);
      }
    }
  }
  else {
    db_query('INSERT INTO {term_hierarchy} (tid, parent) VALUES (%d, %d)', $form_values['tid'], $form_values['parent']);
  }

  db_query('DELETE FROM {term_synonym} WHERE tid = %d', $form_values['tid']);
  if (!empty($form_values['synonyms'])) {
    foreach (explode("\n", str_replace("\r", '', $form_values['synonyms'])) as $synonym) {
      if ($synonym) {
        // Currently changed from chop() to rtrim() as recommended by
        // coder_tough_love.
        db_query("INSERT INTO {term_synonym} (tid, name) VALUES (%d, '%s')", $form_values['tid'], rtrim($synonym));
      }
    }
  }

  // We're OK to run any hooks which are called. If we used
  // drupal_write_record() to insert the record then it would have
  // filled in the tid field with the newly created tid and passed it
  // back out via 'pass by reference'. Since we've already got the
  // tid field filled in as we're creating a new record but with the
  // defined (i.e. specified tid) then then field in $form_values is
  // already filled in.
  if (isset($hook)) {
    module_invoke_all('taxonomy', $hook, 'term', $form_values);
  }

  cache_clear_all();

  return $status;
}
