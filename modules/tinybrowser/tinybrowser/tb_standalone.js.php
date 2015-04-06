<?php
require_once("config_tinybrowser.php");

$tbpath = pathinfo($_SERVER['SCRIPT_NAME']);
$tbmain = $tbpath['dirname'].'/tinybrowser.php';
?>
// check if WYSIWYG editor is used or not 
function check_element(formelementid) {
   try {
      var element = document.getElementById(formelementid);
      if(document.selection) { // IE
         element.focus();
      }
      else { // Other
         var startPos = element.selectionStart;
      }
      return 0; // OK
   }
   catch(err) {
      return 1; // error
   }
}
function tinyBrowserPopUp(type,formelementid,folder) {
   err = check_element(formelementid);
   if(err) {
      alert('This function works with plain textareas only');
      return false;
   }
   tburl = "<?php echo $tbmain; ?>" + "?type=" + type + "&feid=" + formelementid;
   if (folder !== undefined) tburl += "&folder="+folder+"%2F";
   newwindow=window.open(tburl,'tinybrowser','height=<?php echo $tinybrowser['window']['height']+15; ?>,width=<?php echo $tinybrowser['window']['width']+15; ?>,scrollbars=yes,resizable=yes');
   if (window.focus) {newwindow.focus()}
   return false;
}
