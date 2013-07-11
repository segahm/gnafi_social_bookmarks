<?php
header('Content-type: text/css;');
if (!empty($_GET['f']) && preg_match('/^[a-z\-]+$/',$_GET['f'])){
	echo file_get_contents($_SERVER['DOCUMENT_ROOT'].'/scripts/includes/css/'.$_GET['f'].'.css');
}
?>