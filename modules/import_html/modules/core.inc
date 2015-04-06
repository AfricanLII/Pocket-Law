<?php
/**
 * @file
 * Basic per-module functionality used by import_html.
 *
 * Each available module MAY implement a hook_import_html() function to add or
 * manipulate its own data as the node gets saved.
 *
 * This set defines the known core module attributes that can be added to a node
 * when importing. It sets the node (content & title) values, the menu and path
 * values, and possibly some taxonomy tags.
 *
 * Each of these (pretty integral) attributes is set in a modular way as an
 * example for extensions, and to keep the central import_html codebase lighter.
 *
 * Most hooks here are invoked from
 * @see import_html_xhtml_to_node()
 *
 * @ingroup import_html Import HTML
 * @author Dan Morrison http://coders.co.nz/
 */


/**************************************************
 * NODE
 **************************************************/

/**
 * Implementation of hook_import_html
 *
 * Add appropriate normal node attributes - title, body and teaser
 * - to the node
 */
function node_import_html($profile, &$node, $datadoc = NULL) {

  // Setting a few basics will just avoid 'undefined' complaints when previewing
  $node->nid = isset($node->nid) ? $node->nid : 0;
  $node->sticky = isset($node->sticky) ? $node->sticky : FALSE;
  $node->type = isset($node->type) ? $node->type : $profile['content_type'];

  // The preferred filter 'format' of this body is none - not even line breaks.
  $node->format = isset($profile['preferred_filter']) ? $profile['preferred_filter'] : import_html_get_preferred_filter();

  if (!isset($node->language)) {
     // Node of this type has multilingual support enabled?
     if (variable_get('language_content_type_' . $node->type, 0)) {
       $node->language = $profile['import_lang'];
     }
     else {
       $node->language = '';
     }
  }

  // Cleanups

  // Multiple dates? Could have snuck in when classes were imported
  if (isset($node->date) && !is_string($node->date) && ! is_numeric($node->date)) {
    unset($node->date);
  }
  if (isset($node->created) && ! is_numeric($node->created)) {
    import_html_debug("Found a 'created' time %s in the document, converting it to timestamp %t", array('%s' => $node->created, '%t' => strtotime($node->created)), WATCHDOG_INFO);
    $node->created = strtotime($node->created);
  }


  //
  //  Title
  // Note, internally the title should always be plaintext, encoded characters and no newlines
  // A meta 'title' may have been set,
  // but I'll use the real visible one by preference.
  $h1 = xml_get_element_content($datadoc, 'h1');
  if ($h1) {
    $node->title = $h1;
  }
  else {
    $title_elem = xml_get_element_content($datadoc, 'title');
    if ($title_elem) {
      $node->title = $title_elem;
    }
  }
  $node->title = decode_entities($node->title);

  //
  // BODY is the thing with id=content or whatever is set in the prefs
  //
  import_html_debug_code('XML DOM being scanned for XPATH data extraction to find the body content', xml_tostring($datadoc), WATCHDOG_DEBUG);
  $content_element = xml_getelementbyid($datadoc, 'content');

  if (!$content_element) {
    import_html_debug("Failed to find a body, anything with id='content' in this page", array(), WATCHDOG_NOTICE);
    // TODO some cck may be valid without a 'content' body?
  }

  if ($content_element) {
    $node->body = trim(xml_tostring($content_element, TRUE));
    if (empty($node->body)) {
      import_html_debug(
        "Data doc appeared to have a content element, but it was empty. This is probably a problem, unless it's intentional.",
        array(),
        WATCHDOG_WARNING
      );
    }

    // The XSL may give us further hints about how it found this body data.
    // Keep all this for auditing.
    $precision = xml_getattribute($content_element, 'precision');
    if ($precision != NULL) {
      $node->import_html_precision = $precision;
      $node->import_html_precision_explanation = $explanation = xml_getattribute($content_element, 'title');
      import_html_debug(
        "I'm %precision % confident that we found the right body element. !explanation",
        array('%precision' => 100*$precision, '!explanation' => $explanation),
        WATCHDOG_INFO
      );
    }
  } else {
    $node->body = $node->body ? $node->body : "";
  }

  // It's possible that our input was content-encoded (if it came from RSS or the old import-xml node)
  // If so, the entities should be unwrapped.
  // Other (HTML) imports should not require this
  // @TODO when was $node->content_encoded being flagged?
  $node->body = (isset($node->content_encoded)) ? html_entity_decode($node->body):$node->body;

  //
  //  Teaser
  //
  $teaser = xml_textcontent( xml_getelementbyid($datadoc, 'teaser') );
  $description = xml_textcontent( xml_getelementbyid($datadoc, 'description') );
  if ($description && !$teaser) {
    $node->teaser_include = FALSE;
    $teaser = $description ;
  }
  if ($teaser) {
    // We have an explicit, exclusive teaser.
    // This should be handled by NOT joining it with the body.
    $node->teaser_include = FALSE;
    $node->teaser = $teaser;
  }

  // Make up and refine our own teaser
  // TODO work on this a bit more - it's not always what we want
  /*
  // No custom teaser found, but
  // Imported HTML is highly likely to contain formatting that
  // will ruin teasers. Do our own cut-down version that assumes
  // FILTERED HTML
  if (!$teaser) {
    $teaser = filter_filter('process', 0, 1, node_teaser($node->body));
    if ($teaser) $node->teaser = $teaser;
  }

  // Note that if setting our own explicit teaser (and overriding the default process)
  // we must also set the 'do not show in body' flag or we will be creating repetition
  // as the edit form glues it back together.
  */

  // If we don't define our own teaser, Drupal core will guess
  // - and probably get it wrong by a few characters.
  if (! $teaser) {
    $node->teaser = node_teaser($node->body, $node->format);
  }
  // We are not allowed to serialize the check_markup() version of teaser, as that throws calculations off.

}


/**************************************************
 * USER
 **************************************************/

/**
 * Implementation of hook_import_html
 *
 * Add appropriate user identification to the new node.
 */
function user_import_html($profile, &$node, $datadoc = NULL) {
  global $user;
  $node->uid = $user->uid;
  $node->name = $profile['import_user'];
  if ($import_user = user_load(array('name' => $node->name))) {
    $node->uid = $import_user->uid;
  }
  else {
    import_html_debug("Invalid user. User %import_user does not exist. Creating node as current user instead", array('%import_user' => $profile['import_user']), WATCHDOG_WARNING);
  }

  // Experimental
  // If the HTML has a 'creator' or 'author' set, and it's a direct string
  // match with a known user, use that
  $labels = array('creator', 'author'); // 'name' ?
  foreach ($labels as $label) {
    if (!empty($node->$label)) {
      $name = $node->$label;
      if (is_array($name)) {
        $name = array_shift($name);
      }
      if ($import_user = user_load(array('name' => $name))) {
        $node->name = $import_user->name;
        $node->uid = $import_user->uid;
      }
    }
  }
}


/**************************************************
 * MENU
 **************************************************/

/**
 * Menu can't do anything useful in this new-node phase, BUT it can reserve
 * its namespace. Miscellaneous class HTML import may create an attribute called
 * 'menu'. That would be bad.
 */
function menu_import_html($profile, &$node, $datadoc = NULL) {
  unset($node->menu);
}

  //
  // This is currently a freaking mess. I don't know what the menu router is trying to do these days.
  //

/**
 * Set alias and navigation for the given node
 * Helper for import_html_import_files()
 *
 */
function menu_import_html_after_save($profile, &$node, $datadoc = NULL) {
  // Other rules in the template or deduced by third party modules may wish
  // to flag that this page does or does NOT get a menu item, overriding the
  // global profile preference.
  // Flags on the node take priority over the profile.
  // import_html_create_menus , import_html_no_menus
  if ((!$profile['create_menus'] && ! $node->import_html_create_menus) || !empty($node->import_html_no_menus)) {
    return;
  }

  // Invalid configs? need a valid menu path defined
  if (empty($profile['menu_parent_id'])) {
    import_html_debug('Failure setting a menu item. Profile said to create menus but did not define a menu_parent_id', array(), WATCHDOG_WARNING);
    #dpm(get_defined_vars());
    return FALSE;
  }

  $normal_path = "node/{$node->nid}";

  // Create Menu item for this new item
  // Parent menu MAY have been defined during import processes.
  // Override it temporarily
  $menu_parent_id = $profile['menu_parent_id'] = isset($node->menu_parent_id) ? $node->menu_parent_id : $profile['menu_parent_id'];
  list($menu_name, $plid) = explode(':', $menu_parent_id, 2);
  // Scraping may be allowed to provide an attribute called 'link_title'
  // extracted from the XHTML,
  // or label may be provided from elsewhere
  if ( empty($node->link_title) ) {
    $node->link_title = import_html_guess_label($node->title, $node->path);
  }
  import_html_debug("Menu label for '!title' is '!link_title'", array('!title' => $node->title, '!link_title' => $node->link_title), WATCHDOG_DEBUG);
  if (empty($node->link_title)) {
    // This is illegal, we must have a label
    // Getting here should be impossible?
    import_html_debug("No valid label for this menu item. title:'!title' NOT making a menu here", array('!title' => $node->title), WATCHDOG_ERROR);
    #dpm(get_defined_vars());
  }

  // If the title came from HTML, it may include &amp; and things
  $node->link_title = html_entity_decode($node->link_title);

  $menu = NULL;
  if (!empty($node->menu)) {
    $menu = $node->menu;
    // a placeholder or previous menu item existed and was found when the node was loaded.
    import_html_debug("We will re-use the existing menu item, and keep its place in the sitemap");
  }
  else {
    // Check if a placeholder menu for the alias exists.
    // This can happen if the child items were created earlier and required a placeholder
    // Try both canonic and aliased paths
    if ( ! $menu = menu_get_item_by_path($normal_path, $menu_name) ) {
      $menu = menu_get_item_by_path($node->path, $menu_name);
    }
  }
  // $menu is a menu_link array

  // To work nicely, the menu must link to the cannonic node/nid, not the alias.

  // We are likely to need a parent menu trail. Build it - IF there is no valid menu item already available.
  // If we've already found a valid item, don't need to create a parent menu trail
  if (empty($menu)) {
    import_html_debug("Looking for a parent menu item above %path", array('%path' => $node->path), WATCHDOG_DEBUG);
    $parent_menu = import_html_create_menu_path(dirname($node->path), $profile);
    if (! $parent_menu) {
      import_html_debug("Failed to find or even create a parent menu item above %path. We'd better stop trying to build a menu item for this thing then. It could be that the expected menu or menu parent has been deleted.", array('%path' => $node->path), WATCHDOG_DEBUG);
      return FALSE;
    }
  }

  // Check the menu title istn't the same as the parent title
  // Super special edge case. If the parent menu item is identical to the
  // new menu link title, then we don't want that, it's probably a sign of
  // double-ups.
  // In which case, use the node title
  // (which may also be a repeat, but what are we going to do?)
  //
  if ($parent_menu && $node->link_title == $parent_menu['link_title']) {
    import_html_debug("I was about to make a menu item named the same as it's parent [%link_title], but I decided not to, as that would be clutter. Using node title [%node_title] instead.", array('%link_title' => $parent_menu['link_title'], '%node_title' => $node->title), WATCHDOG_INFO);
    $node->link_title = $node->title;
  }

  if (! empty($menu)) {
    // Found a pre-existing menu, update it
    import_html_debug("Discovered that a placeholder menu item for %path already exists. Filling that one out with the full details now", array('%path' => $node->path));
    #dpm($menu);
    // in the case of directory entries being replaced with index.htm versions,
    // the directory label is the dirname, the index page has the real title.
    // So update over top.
    $menu['link_title'] = $node->link_title;
    $menu['hidden'] = 0;
  }
  else {
    // New menu item
    if (empty($parent_menu)) {
      trigger_error("Code should not get here - failed to build menu - no parent menu returned to hang this node off.", E_USER_ERROR);
      return;
    }
    // New menu item
    $menu = array(
      'link_title' => $node->link_title,
      'link_path'  => $normal_path,
      'parent'     => $parent_menu['menu_name'] .':'. $parent_menu['mlid'],
      'menu_name'  => $parent_menu['menu_name'],
      'plid'       => $parent_menu['mlid'],
    );
  }

  if (! $menu['menu_name']) {
    // Illegal value, don't save
    import_html_debug_code("Incomplete menu, cannot save ", $menu , WATCHDOG_ERROR);
    return FALSE;
  }

  $node->menu = $menu;
  import_html_debug_code("About to save menu link ", $menu , WATCHDOG_DEBUG);
  $mlid = menu_link_save($node->menu);
  if (! $mlid) {
    // inconceivable
    import_html_debug("Menu link did not save. Don't know why", array() , WATCHDOG_DEBUG);
  }
  else {
    #$node->menu['mlid'] = $mlid;
  }
  return TRUE;
}


/**
 * Given a traditional URL path, return the parent menu item
 *
 * A helper function to menu_import_html_after_save()
 *
 * This is a recursive function that slices the given
 * (alias) path up until it finds a menu item it recognises.
 * Once found, it comes back down again building a path
 * until we end up with the new parent menu id.
 *
 * If you are creating menu items that do not yet have a parent - the parent
 * menu link cannot go anywhere useful.
 * I'd like to have it fall back to system_admin_menu_block_page by default, but
 * can't see how to make that happen on the fly.
 *
 * @param $path
 *   the  path (aliased URL) we intend to construct
 * @param $title
 *   optional    Display title of the menu item at that path point, otherwist
 * it'll use the last fragment of the path as a display title
 * @return a menu object, the bottom-most, the one we asked for.
 * FALSE if it hit
 * the top unexpectedly
 *
 * @return a menu_link definition ARRAY
 *
 **/
function import_html_create_menu_path($path, $profile = array()) {

  list($menu_name, $plid) = explode(':', $profile['menu_parent_id'], 2);
  $root_parent = array('mlid' => $plid, 'menu_name' => $menu_name, 'link_title' => $menu_name);

  if ((!module_exists('menu')) || (!module_exists('path'))) {
    // It's possible without path.module - but we can't structure them with what we know;
    return $root_parent;
  }
  $strings = array('%path' => $path, '%menu_name' => $menu_name);
  import_html_debug("Looking for a menu item matching path '%path' within %menu_name ", $strings, WATCHDOG_DEBUG);

  // Check parent exists
  if ($menu_link_item = menu_get_item_by_path($path, $menu_name)) {
    // Something goes oddly wrong when paths have arguments?
    // double-check that what we found is in the right menu_name
    if ($menu_link_item['menu_name'] == $menu_name) {
      import_html_debug("Found the menu item for '%path'." . $menu_link_item['mlid'], $strings);
      return $menu_link_item;
    }
    // Else found a matching path - but in an unexpected menu tree.
  }

  import_html_debug("Failed to find a menu item for the URL '%path' in the %menu_name menu. Making it now.", $strings, WATCHDOG_INFO);
  $p_path = dirname($path); // parent path

  if ($p_path == dirname($p_path)) {
    import_html_debug("
        Hit topmost menu path '$p_path', this is either caused by creating a new menu tree
        from the bottom up and we just reached the top,
        or a recursion error (illegal paths).
    ", array(), WATCHDOG_INFO);

    $parent = $root_parent;
  }
  else {
    if (! $parent = menu_get_item_by_path($p_path, $menu_name)) {
      // Parent not found. Need to make it up.
      // Recurse...
      $parent = import_html_create_menu_path($p_path, $profile);
      // ... and welcome back again!
      // That will have climbed up and come back down. with its nearest new ancestor
    }
  }
  // and attach self to it

  if (!$parent) {
    import_html_debug("Failed to regressively build menu for '%path'. This should not happen. Dunno why we are here.", $strings, WATCHDOG_ERROR);
    trigger_error("This code should never run. Something must be wrong in the input", E_USER_ERROR);
    return FALSE;
  }

  if ($path == '.') {
    return $root_parent;
  }

  import_html_debug("Creating a placeholder menu item for %path. Parent was found to be item {$parent['mlid']}", $strings, WATCHDOG_INFO);
  $menu_link_item = import_html_create_placeholder_menu_item($path, $profile, $parent);

  if (empty($menu_link_item)) {
    trigger_error("Failed to create placeholder menu item ($path, $parent) ", E_USER_ERROR);
    return NULL;
  }

  import_html_debug("New Menu Item '". $menu_link_item['title'] ."' id is :". $menu_link_item['mlid']);
  return $menu_link_item;
}

/**
 * Create a dummy node to represent a menu container.
 *
 * This appears to be the only sane way to build a site from the bottom up.
 * I can, If I try, create menu trees that do not link to nodes ... but most
 * users will not be able to remove, over-write or edit them using the tools
 * they are used to.
 *
 * Instead, just make empty shells on-the-fly. You can kill them via content
 * management and things will bubble up and Drupal will not have a hernia.
 *
 * @param $parent is a parent link item array, containing (at least) a mlid and
 * menu_name
 */
function import_html_create_placeholder_menu_item($path, $profile, $parent_menu) {
  if (empty($parent_menu)) {
    import_html_debug('Called ' . __FUNCTION__ . ' without a proper parent menu. Cannot do that, aborting', array(), WATCHDOG_ERROR);
    return FALSE;
  }
  
  // It seems we need to create a dummy node
  // TAKE CARE not to overwrite an existing node though!!
  // with staggered menus, a parent item may actually exist in another tree - do not over-write that!
  $existing_path = drupal_get_normal_path($path);
  if ($existing_path == $path) {
    // Nothing is there yet, so OK, we can add a dummy node
      
    $node = (object) array(
      'type'    => $profile['content_type'],
      'title'   => t('PLACEHOLDER - ') . import_html_guess_label('', $path),
      'path'    => $path,
      'status'  => $profile['import_status'],
      'promote' => FALSE,
      'body' => t("
        Placeholder page.
        Created by import_html.
        This is required to keep the menu in place during imports.
        Feel free to delete it!
      "),
      'pathauto_perform_alias' => FALSE,
    );
    // Set correct owner/user
    user_import_html($profile, $node);
  
    import_html_debug("Inserting placeholder Node !node_link to keep the menu in shape. %hidden", array('!node_link' => l($node->path, $node->path), '%hidden' => (empty($profile['hide_default_menu_placeholders']) ? '' : 'It will be hidden to begin with.') ), WATCHDOG_NOTICE);
    // Ensure the type is valid before saving!
    // Also should prevent overwriting conflicting paths
    import_html_debug_code("Creating placeholder node ", $node , WATCHDOG_DEBUG);
  
    node_save($node);

    if (! $node->nid) {
      trigger_error("Failed to create placeholder node to hold the menus together. Not fatal, but some sections under $path will be unstructured", E_USER_WARNING);
      return NULL;
    }
    $link_path = 'node/'. $node->nid;
  }
  else {
    $link_path = $existing_path;
  }


  // Now make the menu for this dummy node (or whatever the old content was)
  $menu = array(
    'parent'     => $parent_menu['menu_name'] .':'. $parent_menu['mlid'],
    'menu_name'  => $parent_menu['menu_name'],
    'plid'       => $parent_menu['mlid'],
    'link_title' => import_html_guess_label('', $path),
    'link_path'  => $link_path,
  );
  if (! empty($profile['hide_default_menu_placeholders'])) {
    $menu['hidden'] = TRUE;
  }

  $mlid = menu_link_save($menu);

  if (! $mlid) {
    trigger_error("Failed to create placeholder menu link item. Not fatal, but some sections under $path will be unstructured", E_USER_WARNING);
    return NULL;
  }

  $new_menu = menu_link_load($mlid);
  return $new_menu;
}



/**
 * Return a menu item matching a given path or alias.
 * The alias for the requested path will be tried automatically.
 *
 * If the root menu was defined, the lookup will be restricted to that.
 *
 * @return a menu link ARRAY. NULL if no valid menu item was found.
 */
function menu_get_item_by_path($path, $menu_name = NULL) {
  // Caching works against me when adding lots of aliases in one go.
  // Cannot trust get_alias etc
  // Wipe it :(
  //drupal_lookup_path('wipe'); // This is not working?

  // Use DB to fetch all aliases
  $aliases = array($path);
  $placeholders = array(" link_path = '%s' ");
  $result = db_query("SELECT src FROM {url_alias} WHERE dst = '%s'", $path);

  while ($row = db_fetch_array($result)) {
    $aliases[] = $row['src'];
    $placeholders [] = " link_path = '%s' ";
  }

  // It's bad mojo to mess with the DB directly, but menu doesn't provide a lookup API.
  // Or a way to avoid caching. Do it by hand if I need a newly added menu
  $check_menu_name = '';
  if ($menu_name) {
    $check_menu_name = ' AND menu_name = "%s" ';
    $aliases[] = $menu_name;
  }
  $row = db_fetch_array(db_query("SELECT * FROM {menu_links} WHERE link_path <> '' AND (". join($placeholders, 'OR') .") $check_menu_name ", $aliases ));
  if ($row) {
    // If I don't unserialize this, it gets flattened later
    $row['options'] = unserialize($row['options']);
    return $row;
  }
  import_html_debug("menu_get_item_by_path() failed. Failed to find a menu item called '". join($aliases, ', ') ."' $check_menu_name");
  return NULL;

}



/**************************************************
 * PATH
 **************************************************/

/**
 * Path does not actually add the alias here - that is taken care of naturally
 * during node_save. BUT path.module is just not quick enough - it doesn't
 * recognise newly added items, so sometimes find itself making a dozen aliases
 * of the same thing at onec. Force a path flush every round.
 */
function path_import_html($profile, &$node, $datadoc = NULL) {
  drupal_lookup_path('wipe');

  if (empty($node->path)) {
    import_html_debug_code("Given node has no path set. All files must have a path before they can be created, this is an error somewhere.", $node, WATCHDOG_WARNING);
    return FALSE;
  }

  // Deal with funny characters a bit
  $node->path = urldecode($node->path);
}


/**
 * Add path alias support to the import_html process
 *
 * Needed to be run after save as we need to know the nid
 *
 * Adds an old-style (legacy) alias to the node path if required.
 * As path prefers the HIGHER path ID (the later alias added) if choosing
 * between them, we should add the legacy alias before the preferred (short)
 * one.
 */
function path_import_html_after_save($profile, &$node, $datadoc = NULL) {
  #dpm(array('INVOKED path_import_html_after_save for '. $node->path));
  if ($profile['legacy_aliases'] && ($node->old_path != $node->path)) {
    import_html_debug("Setting up URL ALIASES for this item now. Directing {$node->old_path} to go to the system path ". drupal_get_normal_path($node->path), array(), WATCHDOG_DEBUG);
    // trailing slashes should never get this far
    $node->path = rtrim($node->path, '/');
    $normal_path = "node/{$node->nid}";

    if ($normal_path == $node->path) {
      import_html_debug(
        "Failed to resolve %node_path into a system path. Cannot create alias at this time.",
        array('%node_path' => $node->path),
        WATCHDOG_NOTICE
      );
    }
    else {
      // As the preferred path is already set
      // (thanks to node_save() with $node->path earlier)
      // And we need the preferred one to be added LAST,
      // Replace the current alias with the legacy, then re-add the clean one.
      $pid = db_result(db_query("SELECT pid FROM {url_alias} WHERE dst = '%s' AND src = '%s' ", $node->path, $normal_path));
      // If pid returns null, the set_alias will add a new one, otherwise it updates the old one
      if (! db_result(db_query("SELECT count(*) FROM {url_alias} WHERE dst = '%s' ", $node->old_path))) {
        path_set_alias($normal_path, $node->old_path, $pid);
        #dpm("added/updated {$node->old_path}");
      }

      // path_set_alias doesn't really check for duplicates, so we have to check here
      // Or re-runs add too many new ones.
      // drupal_lookup_path('wipe'); // DOES NOT WORK - do it ourselves.
      if (! db_result(db_query("SELECT count(*) FROM {url_alias} WHERE dst = '%s' ", $node->path))) {
        path_set_alias($normal_path, $node->path);
        #dpm("added {$node->path}");
      }

      import_html_debug(
        "This document (known internally as '%normal_path' )
          should now be accessible via aliases as both
          '!main_alias' and '!legacy_alias' ",
        array(
          '%normal_path' => $normal_path,
          '!main_alias' => l($node->path, $node->path),
          '!legacy_alias' => l($node->old_path, $node->old_path)
        ),
        WATCHDOG_NOTICE
      );
    }
  }

}

/**************************************************
 * TAXONOMY
 **************************************************/

/**
 * Publish some extra settings to display on the import_html config page
 *
 * This is a version of {module}_form_{formname}_alter
 *
 * Used to get taxonomy to modify our import_html_profile_settings form and add
 * bits to it
 */
function taxonomy_form_import_html_profile_settings_alter(&$form, $form_state) {
  $profile = $form['#profile'];

  $form['replication']['taxonomy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy'),
  );

  $vocabs = taxonomy_get_vocabularies();
  $freetagging_vocabs = array(0 => t('<none>'));
  foreach ($vocabs as $vid => $vocab) {
    if ($vocab->tags) {
      $freetagging_vocabs[$vid] = $vocab->name;
    }
  }

  $form['replication']['taxonomy']['freetag_vocab'] = array(
    '#title' => t('Absorb keywords as tags'),
    '#type' => 'select',
    '#options' => $freetagging_vocabs,
    '#default_value' => $profile['freetag_vocab'],
    '#description' => t("
       If chosen, any <b>meta keywords</b> or <b>dc:Subject</b> tags
       found in the source will be added as terms in the given vocabulary.
       Works best with <em>freetagging vocabularies</em> that are associated
       with the import node type.
    "),
  );

}

/**
 * hook_import_html_default_settings()
 *
 * Return an array defining the keys that settings need to save.
 * If we define a profile setting of our own, it needs to be listed here to be
 * serialized.
 */
function taxonomy_import_html_default_settings() {
  return array(
    'freetag_vocab' => 0,
  );
}


/**
 * Absorbs elements in the import document we recognise as being (probably)
 * taxonomy terms.
 *
 *
 * Any link rel="tag" is a start. The plaintext content of such a tag is
 * searched for as a term.
 *
 * both <a href rel='tag' > and <link rel="tag"> are good for me.
 *
 * @TODO this still needs to be expanded with real-world cases
 */
function taxonomy_import_html($profile, &$node, $datadoc = NULL) {
  $node->taxonomy = (! empty($node->taxonomy) && is_array($node->taxonomy)) ? $node->taxonomy : array();

  $active_vocabs = taxonomy_get_vocabularies($node->type);
  if (!$active_vocabs) return;

  $freetag_vocab = $profile['freetag_vocab'];

  if ($freetag_vocab && !empty($node->keywords)) {
    $node->taxonomy['tags'][$freetag_vocab] = $node->keywords;
  }
  //TODO support dc:subject also

  // Scan to see if any allow freetagging. After that it's guesswork which one to target. The one with the lowest number;
  $prime_vocab = 0;
  foreach ($active_vocabs as $vocabulary) {
    if ($vocabulary->tags) {
      $prime_vocabulary = $vocabulary;
      break;
    }
  }

  // Translate links with rel=tag into taxonomy terms
  // TODO review and rewrite with some more control form the profile settings.
  // Need to try on some actual use-cases that INCLUDE this info in the first case.
  $relationships = xml_query($datadoc, './/*[@rel]');
  if ($relationships->length > 0) {
    foreach ($relationships as $link) {
      if (empty($link)) continue;
      $reltype = xml_getattribute($link, 'rel');
      // rels can be multiple - like classes. Split on space
      $reltypes = explode(' ', xml_getAttribute($link, 'rel'));
      $rellink = xml_getattribute($link, 'href');
      $label = trim(xml_textcontent($link));
      // Microformats may put their value into a 'title' attribute instead
      if (empty($label)) {
        $label = xml_getAttribute($link, 'title');
      }
      if (!empty($label)) {
        foreach ($reltypes as $reltype) {
          if ($reltype == 'tag') {
            // Some of this magic should probably shift into absorb_metas()
            // Still thinking of the best way to encode tags into the raw HTML

            if (strstr($label, ":")) {
              // allow an extended format for this value
              // to support freetext vocab additions
              // <a rel='tag'>Author:Joe Brown</a> will create a term in the 'Author' vocab called "Joe Brown".
              import_html_debug("Splitting rel link '$label' up to make it a vocab term entry", 2);
              $vocab_term = split(":", $label);
              $vocab = $vocab_term[0];
              $label = $vocab_term[1];
              $vocabulary = taxonomy_get_vocabulary_by_name($vocab);
              if ($vocabulary) {
                $node->taxonomy['tags'][$vocabulary->vid] .= ",". $label;
              }
            }

            // Match against existing terms
            $terms = taxonomy_get_term_by_name($label);
            // Allow an optional 'S' on the end of terms when looking for a match. Just to be flexible.
            $terms = array_merge($terms, taxonomy_get_term_by_name($label ."s"));

            if (!is_array($node->taxonomy)) {
              $node->taxonomy = array();
            }
            if ($terms) {
              foreach ($terms as $term) {
                // If we did successfully identify the vocab earlier, filter on that. Otherwise let anything go
                if ($vocabulary && ($term->vid != $vocabulary->vid)) continue;
                $node->taxonomy[$term->tid] = $term;
              }
            }
            else {
              // Couldn't retrieve it, can we add freetags?
              @$node->taxonomy['tags'][$prime_vocabulary->vid] .= ",". $label;
            }

          } // is a 'tag'
          import_html_absorb_properties($node, $reltype, $label);
        }  // inner rel tag type
      } // has value
    } // each rel
  } // found a rel

  // Tag this new content if the profile has a global tag set
  // May have been set during the form process.
  //
  // Note that if the global tag is from a freetaging vocab, we insert it as a string
  // but if it was from a select category, we insert it by ID.

  $cats = $profile['import_category'];
  if (is_array($cats)) {
    foreach ($cats as $term_id) {
      if ($term = taxonomy_get_term($term_id)) {
        // Be careful not to add terms that are invalid or recently deleted

        // $node->taxonomy contains either strings or IDs.
        // Figure it out and insert into the object accordingly
        // Mixing ids and strings confuses taxonomy submit otherwise
        $term_vocab = taxonomy_vocabulary_load($term->vid);
        if ($term_vocab->tags) {
          @$node->taxonomy['tags'][$term_vocab->vid] .= ', '. $term->name;
        }
        else {
          $node->taxonomy[$term_id] = $term;
        }
      }
      else {
        drupal_set_message("Tried to add the term $cat_id to the node - but it seems to be invalid. Maybe it was deleted. Check the import profile.", 'warning');
      }
    }
  }

}

/**
 * Implimentation of hook_import_html_node_merge()
 *
 * HOOK_import_html_node_merge gets called after a new node has been created,
 * and is being used to update a pre-existing one.
 *
 * A per-module way of blending new values over old ones - conflict resolution
 * and checking for duplicates.
 *
 * If the values are done with, unset them from the $old node so they don't get
 * inadvertantly copied by the bulk operation that follows.
 *
 * Taxonomy needs this because: If I am MERGING with a pre-existing node that
 * already has terms (such as when doing a re-import) then the node taxonomy
 * already has a bunch of identified terms that are likely to be duplicates. I
 * don't want to discard them (they may have been updated in the meantime) but
 * leaving the node with them set, and ALSO adding the freetagging list will
 * cause duplicate complaints. I need to investigate the existing terms and
 * remove them individually. Easiest way is to convert them into a flat list so
 * they get re-saved.
 *
 * taxonomy_preview_terms() didn't look like it will help.
 *
 * taxonomy_form_alter() seems to have the same problem. And solves it sorta.
 *
 * But not good enough for us without rewriting $node->taxonomy totally
 *
 * Taxonomy doesn't mind duplicates in the freetag string, so we will pull the
 * known terms out, just add them to the freetag string, and discard the oldies.
 *
 * It operates on both old and new by reference.
 */
function taxonomy_import_html_node_merge($profile, &$node, &$old_node) {
  if (! isset($old_node->taxonomy)) {
    return;
  }
  foreach ((array)$old_node->taxonomy as $tid => $term) {
    if (is_object($term)) {
      $term_vocab = taxonomy_vocabulary_load($term->vid);
      if ($term_vocab->tags) {
        unset($old_node->taxonomy[$tid]);
        if (! isset($new_node->taxonomy['tags'])) {
          $new_node->taxonomy['tags'] = array();
        }
        if (! isset($new_node->taxonomy['tags'][$term->vid])) {
          $new_node->taxonomy['tags'][$term->vid] = '';
        }
        $new_node->taxonomy['tags'][$term->vid] .= ', '. $term->name;
      }
    }
  }
}


function taxonomy_get_vocabulary_by_name($name) {
  $vs = taxonomy_get_vocabularies();
  foreach ($vs as $voc) {
    if ($voc->name == $name) return $voc;
  }
}


/**************************************************
 * UPLOAD
 **************************************************/

/**
 * Attach embedded files as 'upload' file attachments.
 *
 * We cave found that the imported file refers to some files within its body,
 * eg some embedded images or PDF downloads.
 * These would normally have been handled as Drupal node file attachments. So do
 * that again here.
 *
 *
 */
function upload_import_html($profile, &$node, $datadoc = NULL) {
  #TODO

  // Scan the body for file hrefs and embedded srcs

  // See if entries already exist in the files table

  // See if the file exists locally

  // (Fetch it)

  // Attach it to the node.
}

/**
 * Add a file attachment to a node, given a binary string.
 * Similar to upload.module ... but without the uploading
 */
function upload_import_html_attach_file(&$node, $url, $source, $label = '_original') {
  $filepath = safe_filepath_from_url($url);
  // Probably need to create the directory tree
  mkdirs(dirname($filepath), FILE_CREATE_DIRECTORY);
  file_put_contents($filepath, $source);

  // Attach the file and its info to the node
  // Avoid double-ups be checking to see if it already exists
  $fid = db_result(db_query('SELECT fid FROM {files} WHERE filepath = "%s"', $filepath));

  $info = stat($filepath);
  $file = (object)array(
    'filepath'    => $filepath,
    'filename'    => $label,
    'filesize'    => $info['size'],
    'description' => $label,
    'fid'         => $fid,
    'list'        => $label == '_original',
    'filemime'    => mime_content_type($filepath),
  );
  $node->files[] = $file;
}
