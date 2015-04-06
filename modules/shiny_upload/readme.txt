TO INSTALL:

1/ Drop this folder into your 'modules'  directory (e.g. /sites/all/modules/),
2/ Enable the Shiny_upload module 
3/ Move the file 'upload_attachments.tpl.php' to your theme's folder.

Open a node that contains attachments. 
It should now display the list of uploads in a frienlier manner.

ABOUT:

This module is completely based on Drop-inn override on http://groups.drupal.org/node/13873.
Problem with that 'package' was that it corrupts the cron (due to some function being registered twice).

All credits go to http://groups.drupal.org/user/611, 
Roel has only cleaned up his code. 

TODO:
Get rid of the 3th step. (moving the tpl file to your theme). If anyone has suggestions on how to override the default theming behaviour from within a module. Please contact Roel
 
CONTACT:

Ben Sheldon - bensheldon@gmail.com  - for info on the theming part
Roel De Meester - roel@krimson.be   - for info on the module

December 12, 2008