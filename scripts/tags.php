<?php
require_once($_SERVER['DOCUMENT_ROOT'].'/scripts/includes/interface.inc');
define('IS_INDEX_PAGE',1);
class Tags{
	function __construct(){
		//do a soft user check
		User::checkAuth(false,false);
	}
	public function draw(){
		echo '<div id="tag-page">';
		$parser = new parser($_GET);
		$limit = $parser->getSqlLimit('l',200);
		switch($_GET['tag']){
			case 'popular':
				$this->drawPopular($limit);
				break;
			case 'recent':
				$this->drawRecent($limit);
				break;
			default:
				echo 'tag does not exist';
		}
		echo '</div>';
	}
	private function drawRecent($limit){
		$key_t = new table_keywords();
		$query = 'SELECT '.$key_t->field_keyword.' FROM '.TABLE_KEYWORDS.' GROUP BY '.$key_t->field_keyword
			.' ORDER BY '.$key_t->field_id.' DESC LIMIT '.$limit;
		$result = db::query($query);
		echo '<table width="100%"><td><td align="center"><div align="center" id="tagcloud" style="width:600px;">';
		while($row = db::fetch_row($result)){
			echo '<a href="/p/',$row[0],'" class="tag">',$row[0],'</a> &nbsp;';
		}
		echo '</div></td></tr></table>';
	}
	private function drawPopular($limit){
		$key_t = new table_keywords();
		$query = 'SELECT '.$key_t->field_keyword.',count(*) as c FROM '.TABLE_KEYWORDS.' GROUP BY '.$key_t->field_keyword
			.' ORDER BY c DESC, '.$key_t->field_id.' DESC LIMIT '.$limit;
		$result = db::query($query);
		echo '<table width="100%"><td><td align="center"><div align="center" id="tagcloud" style="width:600px;">';
		while($row = db::fetch_row($result)){
			echo '<a href="/p/',$row[0],'" class="tag">',$row[0],'</a> &nbsp;';
		}
		echo '</div></td></tr></table>';
	}
}
if (!defined('NINCLUDE')){
	db::connect();
	$tags = new Tags();
	if (!isset($_GET['tag']))
			$_GET['tag'] = 'nonexistant';
	switch($_GET['tag']){
		case 'popular':
		$_SERVER['CRUMB_PATH'] = '/popular';
		$_SERVER['CRUMB_ROOT'] = CatPath::crumbRoot($_SERVER['CRUMB_PATH']);
		$_SERVER['CRUMB_WORD'] = CatPath::crumbWord($_SERVER['CRUMB_PATH']); 
		break;
		case 'recent':
		$_SERVER['CRUMB_PATH'] = '/recent';
		$_SERVER['CRUMB_ROOT'] = CatPath::crumbRoot($_SERVER['CRUMB_PATH']);
		$_SERVER['CRUMB_WORD'] = CatPath::crumbWord($_SERVER['CRUMB_PATH']); 
	}
	Template::printSearchHeader();
	$tags->draw();
	Template::printFooter();
	db::close();
}
?>