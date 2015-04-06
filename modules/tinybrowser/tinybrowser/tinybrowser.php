<?php
require_once('config_tinybrowser.php');

// Set language
if(isset($tinybrowser['language']) && file_exists('langs/'.$tinybrowser['language'].'.php'))
	{
	require_once('langs/'.$tinybrowser['language'].'.php'); 
	}
else
	{
	require_once('langs/en.php'); // Falls back to English
	}
require_once('fns_tinybrowser.php');

function show_debug_info() {
	print "---- SESSION ----<br/>\n";
	print "<pre>\n";
	print_r($_SESSION);
	print "</pre>\n";
	print "---- COOKIE ----<br/>\n";
	print "<pre>\n";
	print_r($_COOKIE);
	print "</pre>\n";
	print "session_id is " . session_id() . "<br/>\n";
	print "session_name is " . session_name() . "<br/>\n";
	print "uid is " . $tinybrowser['uid'] . "<br/>\n";
}

// DEBUG
// show_debug_info();

// Check session, if it exists
if(session_id() != '')
	{
	if(!isset($_SESSION[$tinybrowser['sessioncheck']]))
		{
		echo TB_DENIED;
		exit;
		}
	}

// sanitize all input data
if(isset($_GET['type']))
  $_in_type = htmlspecialchars($_GET['type'], ENT_QUOTES);
if(isset($_GET['feid']))
  $_in_feid = htmlspecialchars($_GET['feid'], ENT_QUOTES);
if(isset($_REQUEST['folder']))
  $_in_folder = htmlspecialchars(urldecode($_REQUEST['folder']), ENT_QUOTES);
if(isset($_REQUEST['sortby']))
  $_in_sortby = htmlspecialchars($_REQUEST['sortby'], ENT_QUOTES);
if(isset($_REQUEST['sorttype']))
  $_in_sorttype = htmlspecialchars($_REQUEST['sorttype'], ENT_QUOTES);
if(isset($_REQUEST['viewtype']))
  $_in_viewtype = htmlspecialchars($_REQUEST['viewtype'], ENT_QUOTES);
if(isset($_REQUEST['showpage']))
  $_in_showpage = htmlspecialchars($_REQUEST['showpage'], ENT_QUOTES);
if(isset($_REQUEST['find']))
  $_in_find = htmlspecialchars($_REQUEST['find'], ENT_QUOTES);

// Assign file operation variables
$validtypes = array('image','media','file');
$typenow = ((isset($_in_type) && in_array($_in_type,$validtypes)) ? $_in_type : 'image');
$standalone = ((isset($_in_feid) && !empty($_in_feid)) ? true : false);
$foldernow = str_replace(array('../','..\\','./','.\\'),'',($tinybrowser['allowfolders'] && isset($_in_folder) && !empty($_in_folder) ? $_in_folder : ''));

if($standalone)
	{
	$passfeid = '&feid='. $_in_feid;	
	$rowhlightinit =  ' onload="rowHighlight();"';
	}
else
	{
	$passfeid = '';
	$rowhlightinit =  '';	
	}

// Assign browsing options
$sortbynow = (isset($_in_sortby) && !empty($_in_sortby) ? $_in_sortby : $tinybrowser['order']['by']);
$sorttypenow = (isset($_in_sorttype) && !empty($_in_sorttype) ? $_in_sorttype : $tinybrowser['order']['type']);
$sorttypeflip = ($sorttypenow == 'asc' ? 'desc' : 'asc');  
$viewtypenow = (isset($_in_viewtype) && !empty($_in_viewtype) ? $_in_viewtype : $tinybrowser['view']['image']);
$findnow = (isset($_in_find) && !empty($_in_find) ? $_in_find : false);
$showpagenow = (isset($_in_showpage) && !empty($_in_showpage) ? $_in_showpage : 0);

// Assign url pass variables
$passfolder = '&folder='.urlencode($foldernow);
$passviewtype = '&viewtype='.$viewtypenow;
$passsortby = '&sortby='.$sortbynow.'&sorttype='.$sorttypenow;

// Assign view, thumbnail and link paths
$browsepath = $tinybrowser['path'][$typenow].$foldernow;
$linkpath = $tinybrowser['link'][$typenow].$foldernow;
$thumbpath = $tinybrowser[$tinybrowser['thumbsrc']][$typenow].$foldernow;

// Assign sort parameters for column header links
$sortbyget = array();
$sortbyget['name'] = '&viewtype='.$viewtypenow.'&sortby=name';
$sortbyget['size'] = '&viewtype='.$viewtypenow.'&sortby=size'; 
$sortbyget['type'] = '&viewtype='.$viewtypenow.'&sortby=type'; 
$sortbyget['modified'] = '&viewtype='.$viewtypenow.'&sortby=modified';
$sortbyget['dimensions'] = '&viewtype='.$viewtypenow.'&sortby=dimensions'; 
$sortbyget[$sortbynow] .= '&sorttype='.$sorttypeflip;

// Assign css style for current sort type column
$thclass = array();
$thclass['name'] = '';
$thclass['size'] = ''; 
$thclass['type'] = ''; 
$thclass['modified'] = '';
$thclass['dimensions'] = ''; 
$thclass[$sortbynow] = ' class="'.$sorttypenow.'"';

// Initalise alert array
$notify = array(
	'type' => array(),
	'message' => array()
);
$newthumbqty = 0;

// read folder contents if folder exists
if(file_exists($tinybrowser['docroot'].$browsepath))
	{
	// Read directory contents and populate $file array
	$dh = opendir($tinybrowser['docroot'].$browsepath);
	$file = array();
	while (($filename = readdir($dh)) !== false)
		{
		// get file extension
		$nameparts = explode('.',$filename);
		$ext = end($nameparts);

		// filter directories and prohibited file types
		// if($filename != '.' && $filename != '..' && !is_dir($tinybrowser['docroot'].$browsepath.$filename) && !in_array($ext, $tinybrowser['prohibited']) && ($typenow == 'file' || strpos(strtolower($tinybrowser['filetype'][$typenow]),strtolower($ext))))
		if($filename != '.' && $filename != '..' && !is_dir($tinybrowser['docroot'].$browsepath.$filename) && !in_array($ext, $tinybrowser['prohibited']) && ($typenow == 'file' || in_array($ext, $tinybrowser['filetype'][$typenow])))
			{
			// search file name if search term entered
			if($findnow) $exists = strpos(strtolower($filename),strtolower($findnow));
	
			// assign file details to array, for all files or those that match search
			if(!$findnow || ($findnow && $exists !== false))
				{
				$file['name'][] = $filename;
				$file['sortname'][] = strtolower($filename);
				$file['modified'][] = filemtime($tinybrowser['docroot'].$browsepath.$filename);
				$file['size'][] = filesize($tinybrowser['docroot'].$browsepath.$filename);
	
				// image specific info or general
				if($typenow=='image' && $imginfo = getimagesize($tinybrowser['docroot'].$browsepath.$filename))
					{
					$file['width'][] = $imginfo[0];
					$file['height'][] = $imginfo[1];
					$file['dimensions'][] = $imginfo[0] + $imginfo[1];
					$file['type'][] = $imginfo['mime'];
					
					// Check a thumbnail exists
					if(!file_exists($tinybrowser['docroot'].$browsepath.'_thumbs/')) createfolder($tinybrowser['docroot'].$browsepath.'_thumbs/',$tinybrowser['unixpermissions']);
			  		$thumbimg = $tinybrowser['docroot'].$browsepath.'_thumbs/_'.$filename;
					if (!file_exists($thumbimg))
						{
						$nothumbimg = $tinybrowser['docroot'].$browsepath.$filename;
						$mime = getimagesize($nothumbimg);
						$im = convert_image($nothumbimg,$mime['mime']);
						resizeimage($im,$tinybrowser['thumbsize'],$tinybrowser['thumbsize'],$thumbimg,$tinybrowser['thumbquality'],$mime['mime']);
						imagedestroy($im);
						$newthumbqty++;
						}
					}
				else 
					{
					$file['width'][] = 'N/A';
					$file['height'][] = 'N/A';
					$file['dimensions'][] = 'N/A';
					$file['type'][] = returnMIMEType($filename);
					}
				}			
			}
		}
	closedir($dh);
	}
// create file upload folder
else
	{
	$success = createfolder($tinybrowser['docroot'].$browsepath,$tinybrowser['unixpermissions']);
	if($success)
		{
		if($typenow=='image') createfolder($tinybrowser['docroot'].$browsepath.'_thumbs/',$tinybrowser['unixpermissions']);
		$notify['type'][]='success';
		$notify['message'][]=sprintf(TB_MSGMKDIR, $browsepath);
		}
	else
		{
		$notify['type'][]='error';
		$notify['message'][]=sprintf(TB_MSGMKDIRFAIL, $browsepath);
		}
	}
	
// Assign directory structure to array
if($tinybrowser['allowfolders']) {
	$browsedirs=array();
	dirtree($browsedirs,$tinybrowser['filetype'][$typenow],$tinybrowser['docroot'],$tinybrowser['path'][$typenow]);
}
	
// generate alert if new thumbnails created
if($newthumbqty>0)
   {
	$notify['type'][]='info';
	$notify['message'][]=sprintf(TB_MSGNEWTHUMBS, $newthumbqty);
	}
	

// determine sort order
$sortorder = ($sorttypenow == 'asc' ? SORT_ASC : SORT_DESC);
$num_of_files = (isset($file['name']) ? count($file['name']) : 0);

if($num_of_files>0)
	{
	// sort files by selected order
	sortfileorder($sortbynow,$sortorder,$file);
	}

// determine pagination
if($tinybrowser['pagination']>0)
	{
    $curpage = (isset($_in_showpage) ? intval($_in_showpage) :  1);
	$showpage_start = ($showpagenow ? (($curpage-1)*$tinybrowser['pagination']) : 0);
	$showpage_end = $showpage_start+$tinybrowser['pagination'];
	if($showpage_end>$num_of_files) $showpage_end = $num_of_files;
	}
else
	{
	$showpage_start = 0;
	$showpage_end = $num_of_files;
	}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>TinyBrowser :: <?php echo TB_BROWSE; ?></title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta http-equiv="Pragma" content="no-cache" />
<?php
if(!$standalone && $tinybrowser['integration']=='tinymce')
	{
	?><script language="javascript" type="text/javascript" src="<?php echo $tinybrowser['tinymcepop']; ?>"></script>
	<link rel="stylesheet" type="text/css" media="all" href="<?php echo $tinybrowser['tinymcecss']; ?>" /><?php
	}
else
	{
	?><link rel="stylesheet" type="text/css" media="all" href="css/stylefull_tinybrowser.css" /><?php 
	}
?>
<link rel="stylesheet" type="text/css" media="all" href="css/style_tinybrowser.css.php" />
<script language="javascript" type="text/javascript" src="js/tinybrowser.js.php?<?php echo substr($passfeid,1); ?>"></script>
</head>
<body<?php echo $rowhlightinit; ?>>
<?php
if(count($notify['type'])>0) alert($notify);
form_open('foldertab',false,'tinybrowser.php','?type='.$typenow.$passviewtype.$passsortby.$passfeid);
?>
<div class="tabs">
<ul>
<li id="browse_tab" class="current"><span><a href="tinybrowser.php?type=<?php echo $typenow.$passfolder.$passfeid; ?>"><?php echo TB_BROWSE; ?></a></span></li><?php
if($tinybrowser['allowupload']) 
	{
	?><li id="upload_tab"><span><a href="upload.php?type=<?php echo $typenow.$passfolder.$passfeid; ?>"><?php echo TB_UPLOAD; ?></a></span></li><?php
	}
if($tinybrowser['allowedit'] || $tinybrowser['allowdelete'])
	{
	?><li id="edit_tab"><span><a href="edit.php?type=<?php echo $typenow.$passfolder.$passfeid; ?>"><?php echo TB_EDIT; ?></a></span></li><?php
	}
if($tinybrowser['allowfolders'])
	{
	?><li id="folders_tab"><span><a href="folders.php?type=<?php echo $typenow.$passfolder.$passfeid; ?>"><?php echo TB_FOLDERS; ?></a></span></li><?php

	// Display folder select, if multiple exist
	if(count($browsedirs)>1)
		{
		?><li id="folder_tab" class="right"><span><?php
		form_select($browsedirs,'folder',TB_FOLDERCURR,urlencode($foldernow),true);
		?></span></li><?php
		} 
	}
?>
</ul>
</div>
</form>
<div class="panel_wrapper">
<div id="general_panel" class="panel currentmod">
<fieldset>
<legend><?php echo TB_BROWSEFILES; ?></legend>
<?php
form_open('browse','custom','tinybrowser.php','?type='.$typenow.$passfolder.$passfeid);
?>
<div class="pushleft">
<?php

// Offer view type if file type is image
if($typenow=='image')
	{
	$select = array(
		array('thumb',TB_THUMBS),
		array('detail',TB_DETAILS)
	);
	form_select($select,'viewtype',TB_VIEW,$viewtypenow,true);
	}
	
// Show page select if pagination is set
if($tinybrowser['pagination']>0)
	{
	$pagelimit = ceil($num_of_files/$tinybrowser['pagination'])+1;
	$page = array();
	for($i=1;$i<$pagelimit;$i++)
		{
		$page[] = array($i,TB_PAGE.' '.$i);
		}
	if($i>2) {
      form_select($page,'showpage',TB_SHOW,$showpagenow,true);
      if($curpage <= 0) $curpage = 1;
      $prev = $curpage-1;
      $next = $curpage+1;
      if($prev <= 0) $prev = 1;
      if($next > ($pagelimit-1)) $next = ($pagelimit-1);
      $base_url = '?type=' . $typenow.$passfolder.$passfeid.$passviewtype.$passsortby;
      if($findnow) $base_url .= '&find=' . $findnow;
      if($prev != $curpage) {
	    print '<a href="' . $base_url . '&showpage=' . $prev . '"><img src="img/prev.png" border="0" class="prevp" /></a>';
      }
      else {
        print '<img src="img/prev-disable.png" border="0" class="prevp" />';
      }
      if($next != $curpage) {
	    print '<a href="' . $base_url . '&showpage=' . $next . '"><img src="img/next.png" border="0" class="nextp" /></a>';
      }
      else {
        print '<img src="img/next-disable.png" border="0" class="nextp" />';
      }
	}
}
?></div><div class="pushright"><?php

form_hidden_input('sortby',$sortbynow);
form_hidden_input('sorttype',$sorttypenow);
form_text_input('find',false,$findnow,25,50);
form_submit_button('search',TB_SEARCH,'');

?></div>
<?php

// if image show dimensions header
if($typenow=='image')
	{
	$imagehead = '<th><a href="?type='.$typenow.$passfolder.$passfeid.$sortbyget['dimensions'].'"'.$thclass['dimensions'].'>'.TB_DIMENSIONS.'</a></th>';
	}
else $imagehead = '';

echo '<div class="tabularwrapper"><table class="browse">'
		.'<tr><th><a href="?type='.$typenow.$passfolder.$passfeid.$sortbyget['name'].'"'.$thclass['name'].'>'.TB_FILENAME.'</a></th>'
		.'<th><a href="?type='.$typenow.$passfolder.$passfeid.$sortbyget['size'].'"'.$thclass['size'].'>'.TB_SIZE.'</a></th>'
		.$imagehead
		.'<th><a href="?type='.$typenow.$passfolder.$passfeid.$sortbyget['type'].'"'.$thclass['type'].'>'.TB_TYPE.'</th>'
		.'<th><a href="?type='.$typenow.$passfolder.$passfeid.$sortbyget['modified'].'"'.$thclass['modified'].'>'.TB_DATE.'</th></tr>';

// show image thumbnails, unless detail view is selected
if($typenow=='image' && $viewtypenow != 'detail')
	{
	echo '</table></div>';

	for($i=$showpage_start;$i<$showpage_end;$i++)
		{
        $file_url=$linkpath.$file['name'][$i];
		if($tinybrowser['absolute_url']) {
        	$file_url = $tinybrowser['host'] . $file_url;
		}
		echo '<div class="img-browser"><a href="#" onclick="selectURL(\''.$typenow.'\',\''.$file_url.'\',\''.$file['name'][$i].'\',\''.$file['width'][$i].'\',\''.$file['height'][$i].'\',\''.bytestostring($file['size'][$i],1).'\');" title="'.TB_FILENAME.': '.$file['name'][$i]
				.'&#13;&#10;'.TB_DIMENSIONS.': '.$file['width'][$i].' x '.$file['height'][$i]
				.'&#13;&#10;'.TB_DATE.': '.date($tinybrowser['dateformat'],$file['modified'][$i])
				.'&#13;&#10;'.TB_TYPE.': '.$file['type'][$i]
				.'&#13;&#10;'.TB_SIZE.': '.bytestostring($file['size'][$i],1)
				.'"><img src="'.$thumbpath.'_thumbs/_'.$file['name'][$i]
				.'"  /><div class="filename">'.$file['name'][$i].'</div></a></div>';
		}
	}
else
	{
	for($i=$showpage_start;$i<$showpage_end;$i++)
		{
        $file_url=$linkpath.$file['name'][$i];
		if($tinybrowser['absolute_url']) {
        	$file_url = $tinybrowser['host'] . $file_url;
		}
		$alt = (IsOdd($i) ? 'r1' : 'r0');
		echo '<tr class="'.$alt.'">';
		if($typenow=='image') echo '<td><a class="imghover" href="#" onclick="selectURL(\''.$typenow.'\',\''.$file_url.'\',\''.$file['name'][$i].'\',\''.$file['width'][$i].'\',\''.$file['height'][$i].'\',\''.bytestostring($file['size'][$i],1).'\');" title="'.$file['name'][$i].'"><img src="'.$thumbpath.'_thumbs/_'.$file['name'][$i].'" alt="" />'.truncate_text($file['name'][$i],30).'</a></td>';
		else echo '<td><a href="#" onclick="selectURL(\''.$typenow.'\',\''.$file_url.'\',\''.$file['name'][$i].'\',\''.$file['width'][$i].'\',\''.$file['height'][$i].'\',\''.bytestostring($file['size'][$i],1).'\');" title="'.$file['name'][$i].'">'.truncate_text($file['name'][$i],30).'</a></td>';
		echo '<td>'.bytestostring($file['size'][$i],1).'</td>';
		if($typenow=='image') echo '<td>'.$file['width'][$i].' x '.$file['height'][$i].'</td>';	
		echo '<td>'.$file['type'][$i].'</td>'
			.'<td>'.date($tinybrowser['dateformat'],$file['modified'][$i]).'</td></tr>'."\n";
		}
	echo '</table></div>';
	}
?>
</fieldset></div></div>
<form name="passform"><input name = "fileurl" type="hidden" value= "" /></form>
</body>
</html>
