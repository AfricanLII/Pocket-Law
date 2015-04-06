<?php
require_once('config_tinybrowser.php');
require_once('fns_tinybrowser.php');

// delay script if set
if($tinybrowser['delayprocess']>0) sleep($tinybrowser['delayprocess']);

// sanitize all input data
if(isset($_GET['type']))
  $_in_type = htmlspecialchars($_GET['type'], ENT_QUOTES);
if(isset($_GET['feid']))
  $_in_feid = htmlspecialchars($_GET['feid'], ENT_QUOTES);
if(isset($_GET['folder']))
  $_in_folder = htmlspecialchars(urldecode($_GET['folder']), ENT_QUOTES);
if(isset($_GET['filetotal']))
  $_in_filetotal = htmlspecialchars($_GET['filetotal'], ENT_QUOTES);

// Initialise files array and error vars
$files = array();
$good = 0;
$bad = 0;
$dup = 0;
$total = (isset($_in_filetotal) ? intval($_in_filetotal) : 0);

// Assign get variables
$validtypes = array('image','media','file');
$typenow = ((isset($_in_type) && in_array($_in_type,$validtypes)) ? $_in_type : 'image');
$folder = $tinybrowser['docroot']. $_in_folder;

$foldernow = str_replace($tinybrowser['path'][$typenow],'', $_in_folder);
$passfeid = (isset($_in_feid) ? '&feid='. $_in_feid : '');

$cnt = 0;

if ($handle = opendir($folder)) {
	while (false !== ($file = readdir($handle)))
	{
		if ($file != "." && $file != ".." && substr($file,-1)=='_') {
			//-- File Naming
			$tmp_filename = $folder.$file;
			$dest_filename	 = $folder.rtrim($file,'_');
            // watchdog('tinybrowser', 'tmp_filename[!tmp], dest_filename[!dest]', array('!tmp' => $tmp_filename, '!dest' => $dest_filename));
        
			//-- Duplicate Files
			if(file_exists($dest_filename)) { 
				if($tinybrowser['upload_mode'] == 3) {
					// keep the existing one, reject the new one
					unlink($tmp_filename); $dup++; continue; 
                }
				else if($tinybrowser['upload_mode'] == 2) {
					// keep the existing one, rename the new one
					for($idx = 1 ; ; $idx++) {
						$data = pathinfo(rtrim($file,'_'));
						$dest_filename = 
							$folder.$data['filename']
							.'-'.$idx
							.'.'.$data['extension'];
						if(!file_exists($dest_filename)) break;
					}
				}
			}
			//-- Bad extensions
			$nameparts = explode('.',$dest_filename);
			$ext = end($nameparts);
			
			if(!validateExtension($ext, $tinybrowser['prohibited'])) { 
				unlink($tmp_filename); continue; 
			}
        
			//-- Rename temp file to dest file
			rename($tmp_filename, $dest_filename);
			$good++;
			
			//-- if image, perform additional processing
			if($typenow == 'image') {

				//-- Good mime-types
				$imginfo = getimagesize($dest_filename);
	   			if($imginfo === false) { unlink($dest_filename); continue; }
				$mime = $imginfo['mime'];

				// resize image to maximum height and width, if set
				if($tinybrowser['imageresize']['width'] > 0 || $tinybrowser['imageresize']['height'] > 0)
				{
					// assign new width and height values, only if they are less than existing image size
					$widthnew  = ($tinybrowser['imageresize']['width'] > 0 && $tinybrowser['imageresize']['width'] < $imginfo[0] ? $tinybrowser['imageresize']['width'] : $imginfo[0]);
					$heightnew = ($tinybrowser['imageresize']['height'] > 0 && $tinybrowser['imageresize']['height'] < $imginfo[1] ? $tinybrowser['imageresize']['height'] :  $imginfo[1]);

					// only resize if width or height values are different
					if($widthnew != $imginfo[0] || $heightnew != $imginfo[1])
					{
						$im = convert_image($dest_filename,$mime);
						resizeimage($im,$widthnew,$heightnew,$dest_filename,$tinybrowser['imagequality'],$mime);
						imagedestroy($im);
					}
				}
				// generate thumbnail
				$thumbimg = $folder.'_thumbs/_'.rtrim($file,'_');
				if (!file_exists($thumbimg) || $tinybrowser['upload_mode'] == 1)
				// if (!file_exists($thumbimg))
				{
					$im = convert_image($dest_filename,$mime);
					resizeimage	($im,$tinybrowser['thumbsize'],$tinybrowser['thumbsize'],$thumbimg,$tinybrowser['thumbquality'],$mime);
					imagedestroy ($im);
				}
      		}
		}
        else {
        }
	}
	closedir($handle);
}
$bad = $total-($good+$dup);

// Check for problem during upload
if($total>0 && $bad==$total) {
  // NG: failed to upload
  // watchdog('tinybrowser', 'upload_process failed: total=!total, good=!good, dup=!dup, bad=!bad', array('!total' => $total, '!good' => $good, '!dup' => $dup, '!bad' => $bad));
  Header('Location: ./upload.php?type='.$typenow.$passfeid.'&permerror=1&total='.$total);
}
else {
  // OK: succeeded
  Header('Location: ./upload.php?type='.$typenow.$passfeid.'&folder='.$foldernow.'&badfiles='.$bad.'&goodfiles='.$good.'&dupfiles='.$dup);
}
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<meta http-equiv="Pragma" content="no-cache" />
		<title>TinyBrowser :: Process Upload</title>
	</head>
	<body>
		<p>Sorry, there was an error processing file uploads.</p>
	</body>
</html>
