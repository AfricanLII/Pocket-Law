
CONTENTS OF THIS FILE
---------------------

 * Overview
 * Installation
 * Usage

OVERVIEW
--------

The purpose of this module is to provide a way to move data between
Drupal instances. 

This is not easy to achieve as Drupal stores configuration and data in
the same database. Also, Drupal can be extended and customised which
makes creating a standard data export/import module effectively
impossible. 

This module will provide a framework for exporting datasets as files
and also for importing these dataset files. That way if the module is
installed on two separate Drupal instances it will be possible to
export data as a dataset file from one instance and import it into a
different Drupal instance. 

This module will be customisable and extendable by adding new profiles
to take into account different Drupal builds. This means that if a
Drupal instance has a very specialised setup it will be possible to
create a new dataset profile to export/import data. 

The module currently includes two initial dataset profiles which can
be used on standard Drupal instances. These enable the following
datasets to be exported/imported.

 * Taxonomy vocabulary terms
 * Pages

When exporting all data should be exported - when importing the data
should be used in a way to make sure that the receiving Drupal
instance ends up with matching data. Careful checks will be needed to
ensure ID's match, that the data remains consistent.

Effectively this module will be similar to the backup_migrate module
except that the backup_migrate profiles will be extended. Currently
the backup_migrate profiles only allow for the choice of which tables
will be exported/imported and a few other settings. This module will
use the Druapl API to read data and produce rich data files which will
contain all data needed to reproduce a particular dataset. It will
also use the Drupal API to recreate data. It may also be necessary to
use SQL directly to export/import data.

The primary usage for this module is envisioned to be exporting
datasets from a live Drupal site and importing this live data into a
new updated version of the site which developers have been working
on - i.e. a beta version. Once the import has completed then the newly
developed site can become the new live site. This has the advantage of
being able to fall back to the original live site if there is an
unforseen problem between the new version of the site and the live
data.

It will be necessary to carry out proper checks when importing
datasets. E.g.. when node data is being imported it will be necessary
to check that the receiving site has the correct content type and
fields etc. It may be necessary to use the UUID module to provide
unique ID's.

INSTALLATION
------------

 * Put the module in your Drupal modules directory and enable it in
   admin/build/modules.
 * Go to admin/user/permissions and grant permission to any roles that
   need to be able to export or import datasets.

USAGE
-----

 * Configure and use the module at admin/content/data_export_import.
 * The dataset files are exported to and imported from
   {files}/data_export_import.  Various methods can be used to manage
   the files and to move the files between Drupal instances.
   Although not tested the module at:
   http://drupal.org/project/webfm looks suitable for file management.
 * The user which is running the module needs to have permissions to
   be able to delete nodes when importing pages.  It is expected that
   data exporting and importing will be carried out by the admin user
   which has correct permissions.
 * Currently the pages import has been created to import standard
   simple pages.  It can handle a simple added text type CCK field -
   but any other file type fields are not being handled yet.
 * If you have taxonomy terms associated with pages this has not been
   coded for yet.  It may work OK but you might be better to import
   the taxonomy terms separately first and then import the pages.

NOTES
-----

 * This module is not compatible with any site which has localisation
   enabled. Checks have been put in place to prevent data being output
   if localisation has been installed and enabled using the i18n
   module.
 * If this is a requirement then please get in touch with the
   maintainer.
