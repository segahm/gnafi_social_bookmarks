<?php
if ($resultSet != null && $row = db::fetch_row($resultSet)){
	assert(isset($field_keys));
	assert(isset($url_t));
	assert(isset($users_t));
	assert(isset($parser));
	assert(isset($options));
	//make the above global
	$GLOBALS['url_t'] = $url_t;
	$GLOBALS['users_t'] = $users_t;
	$GLOBALS['field_keys'] = $field_keys;
	$GLOBALS['options'] = $options;
	echo '<div id="indexlinks"><ol>';
	$count = 0;
	do{
		printUrl($row);
		++$count;
	}while($row = db::fetch_row($resultSet));
	echo '</ol></div>';
	if (!isset($options['nopages']))
	echo '<p align="center">',$parser->getSqlPages($count),'</p>';
}else{
	echo '<p>no results were found</p>';
}
function printUrl($row){
	global $url_t,$field_keys,$users_t,$options;
	//$field_keys are inverted keys, such that to get a value we use: 
	//$row[$field_keys[$url_t->field...]] - this elimates the problem with when fetch_assoc
	//returns field names without table names
	$title = toHTML($row[$field_keys[$url_t->field_title]]);
	$link = toHTML($row[$field_keys[$url_t->field_url]]);
	if (!empty($row[$field_keys[$url_t->field_keywords]])){
		$tags = CatPath::explodeStrToKeywords($row[$field_keys[$url_t->field_keywords]]);
		$count = 0;
		foreach($tags as $k => $t){
			if ($count == 4){
				$tags = array_slice($tags,0,4);
				$tags[] = '<span class="tag">...</span>';
				break;
			}
			$tags[$k] = '<a href="/p/'.urlencode($t).'" id="history-link-'.$row[$field_keys[$url_t->field_id]].'-'.$k.'" class="tag">'.$t.'</a>';
			++$count;
		}
	}
	echo '<li id="link-',$row[$field_keys[$url_t->field_id]],'" class="item"><span class="header"><a class="bk-link" id="history-link-'.$row[$field_keys[$url_t->field_id]].'" href="',$link,'">',
		$title,'</a>',(isset($options['edit'])?'&nbsp;<span class="bk-edit-delete"><a href="http://" onclick="Edit.edit(this,\''.$row[$field_keys[$url_t->field_id]].'\','.(is_null($row[$field_keys[$url_t->field_insync]])?'null':"'{$row[$field_keys[$url_t->field_insync]]}'").');return false;">edit</a>/'
		.'<a href="http://" onclick="Edit.confirmDel(this,\''.$row[$field_keys[$url_t->field_id]].'\');return false;">delete</a></span>':''),
		(isset($field_keys[$users_t->field_username])?('<span class="span"> by <a class="by" href="'
			.$_SERVER['REQUEST_URI'].((strpos($_SERVER['REQUEST_URI'],'?') !== false)?'&who=':'?who=')
			.$row[$field_keys[$users_t->field_username]].'">'
			.$row[$field_keys[$users_t->field_username]].'</a></span> '.((isset($field_keys['others']) && (($row[$field_keys['others']] = (count(explode(' ',$row[$field_keys['others']]))-1)) > 0))?'<span style="font-size:11px;font-weight:normal;color:#DFC3C9">and '.$row[$field_keys['others']].' other[s]</span><br/>':'').' :: '):''),
		(!is_null($row[$field_keys[$url_t->field_isfeed]])?' <a href="livefeed://path='
		.preg_replace('-[_+]-','%20',urlencode((($_SERVER['CRUMB_PATH'] != '/')?$_SERVER['CRUMB_PATH']:'').'/'.$title))
		.'&url='.urlencode($link).'"><img src="/images/rss_feed.gif"/></a>&nbsp;':''),
		'</span>',(isset($tags)?implode('',$tags):''),(!empty($row[$field_keys[$url_t->field_description]])?'<p id="history-link-desc-'.$row[$field_keys[$url_t->field_id]].'" class="description">'
			.toHTML(funcs_cutLength($row[$field_keys[$url_t->field_description]],200)).'</p>':''),
			(isset($options['edit'])?'<input type="hidden" id="link-pv-'.$row[$field_keys[$url_t->field_id]].'" value="'
				.(is_null($row[$field_keys[$url_t->field_private]])?'0':'1').'"/>':'')
			,'</li>';
}
?>