<?php
class UserOut{
	const gnRdf = 'http://gnafi#';
	const nsRdf = 'http://home.netscape.com/NC-rdf#';
	const rdfRdf = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
	private $userid;
	function __construct($userid){
		$this->userid = $userid;
	}
	/*public function latestUserRdf2($userid){
		try{
			$sync_t = new table_synclatest();
			$query = 'SELECT '.$sync_t->field_actpath.','.$sync_t->field_info.' FROM '
				.TABLE_SYNCLATEST.' WHERE '.$sync_t->field_userid." = '".$userid."'";
			$result = db::query($query);
			$xml = new DOMDocument('1.0','UTF-8');
			$str = <<<EOD
<?xml version="1.0"?><RDF xmlns:NC="http://home.netscape.com/NC-rdf#" 
xmlns="http://www.w3.org/1999/02/22-rdf-syntax-ns#" 
xmlns:gn="http://gnafi#">
<Description about="http://gnafi#root"/>
</RDF>
EOD;
			$xml->loadXML($str);
			$indexes = array();	//keeps hashed path indexes of nodes for all previous folders
			$rootRdf = $xml->getElementsByTagName('RDF')->item(0);
			$root = $xml->getElementsByTagName('Description')->item(0);
			while($row = db::fetch_row($result)){
				$info = CatPath::extractUrlInfo($row[1]);
				//append each one of the properties
				if (isset($info['URL'])){
					$urlv = $info['URL'];
					unset($info['URL']);
					$type = 'gn:URL';
				}else{
					$urlv = $info['FeedURL'];
					unset($info['FeedURL']);
					$type = 'gn:Feed';
				}
				$el = $xml->createElement($type);
				$urlabout = '#$'.genIDString(6);
				$el->setAttribute('about',self::gnRdf.$urlabout);
				foreach($info as $name => $value){
					$newel = $xml->createElement('NC:'.$name,$value);
					$el->appendChild($newel);
				}
				
				//now we need to find the parent to which to attach the element
				if ($row[0] == null){
					//url resource -> u:parent's about -> $url literal
					$el->appendChild($xml->createElement('gn:root',$urlv));
					$temp = $xml->createElement('gn:li');
					$temp->appendChild($el);
					$root->appendChild($temp);
				}else{
					if (isset($indexes[$row[0]])){
						$el->appendChild($xml->createElement('gn:'.$indexes[$row[0]][0],$urlv));
						$temp = $xml->createElement('gn:li');
						$temp->appendChild($el);
						$indexes[$row[0]][1]->appendChild($temp);
					}else{
						//create a new folder
						$res = 'root';
						$resource = $root;
						$chain = explode('/',$row[0]);
						$path = '';
						foreach($chain as $name){
							$path .= $name;
							{
							if (!isset($indexes[$path])){
								$about = '#$'.genIDString(6);
								//create a description element
								$node = $xml->createElement('Description');
								$node->setAttribute('about',self::gnRdf.$about);
								//create a relationship with a parent
								$node->appendChild($xml->createElement('gn:'.$res,$name));
								//reference from the parent to the child
								$temp = $xml->createElement('gn:li');
								$temp->appendChild($node);						
								$resource->appendChild($temp);
								$vals = array();
								$vals[0] = $about;
								$vals[1] = $node;	//save the copy not the reference
								$indexes[$path] = $vals;
								$res =& $vals[0];
								$resource =& $vals[1];
							}else{
								$found =& $indexes[$path];							
								$res =& $found[0];
								$resource =& $found[1];
							}
							}
							$path .= '/';
						}
						$el->appendChild($xml->createElement('gn:'.$res,$urlv));
						$temp = $xml->createElement('gn:li');
						$temp->appendChild($el);		
						$resource->appendChild($temp);
					}
				}
			}
			return $xml->saveXML();
		}catch(Exception $e){
			error_log('failed outputing latest revision:'.$e->getLine().':'.$e->getMessage().':'.$e->getCode());
			throw $e;
		}
	}*/
	public function CvsOnlyRdf(&$checkedindxml,$cvsid,$node_length){
		$sync_t = new table_synclatest();
		$userid = $this->userid;
		//first get all values that we need to add
		$query = 'SELECT '.$sync_t->field_actpath.','.$sync_t->field_info.' FROM '
			.TABLE_SYNCLATEST.' WHERE '.$sync_t->field_userid." = '".$userid."' AND "
			.$sync_t->field_flag.' IS NULL ORDER BY '
			.$sync_t->field_actpath;
		$result = db::query($query);
		$xml = new DOMDocument('1.0','UTF-8');
		$rootNode = $this->latestRdf($xml,$result,$checkedindxml);
		db::free_result($result);
		$result = null;
		$query = 'UPDATE '.TABLE_SYNCLATEST.' SET '.$sync_t->field_flag.' = NULL,'
			.$sync_t->field_cvsid.' = '.toSQL($cvsid).' WHERE '	
			.$sync_t->field_userid." = '".$userid."' ORDER BY ".$sync_t->field_flag
			.' DESC LIMIT '.$node_length;
		db::query($query);
		//now get all values that we need to remove
		$temp_t = new table_synctemp();
		$query = 'SELECT '.TABLE_SYNCTEMP.'.'.$temp_t->field_aboutid.' FROM '.TABLE_SYNCTEMP
			.' LEFT JOIN '.TABLE_SYNCLATEST.' ON '.TABLE_SYNCTEMP.'.'.$temp_t->field_hashkey.' = '
			.TABLE_SYNCLATEST.'.'.$sync_t->field_hashkey.' AND '
			.TABLE_SYNCLATEST.'.'.$sync_t->field_userid.' = '
			.TABLE_SYNCTEMP.'.'.$temp_t->field_userid.' WHERE '
			.TABLE_SYNCLATEST.'.'.$sync_t->field_hashkey.' IS NULL AND '
			.TABLE_SYNCTEMP.'.'.$temp_t->field_userid." = '".$userid."'";
		$result = db::query($query);
		//append to the end of the response
		$li = $xml->createElement('li');
		$seq = $xml->createElement('Seq');
		$seq->setAttribute('about',self::gnRdf.'Removed');
		$parent = $li->appendChild($seq);
		$rootNode->appendChild($li);
		while($row = db::fetch_row($result)){
			$li = $xml->createElement('li');
			$parent->appendChild($li);
			$li->appendChild($xml->createTextNode($row[0]));
		}
		db::free_result($result);
		return $xml->saveXML();
	}
	public function latestEntriesRdf(){
		$sync_t = new table_synclatest();
		$query = 'SELECT '.$sync_t->field_actpath.','.$sync_t->field_info.' FROM '
			.TABLE_SYNCLATEST.' WHERE '.$sync_t->field_userid." = '".$this->userid."' ORDER BY "
			.$sync_t->field_actpath;
		$result = db::query($query);
		$xml = new DOMDocument('1.0','UTF-8');
		$rootNode = $this->latestRdf($xml,$result);
		return $xml->saveXML();
	}
	
	private function latestRdf(&$xml,$result,$doc = null){
		try{
			$sync_t = new table_synclatest();
			$cur_time = time();
			$out = <<<EOD
<?xml version="1.0"?>
<RDF:RDF xmlns="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:NC="http://home.netscape.com/NC-rdf#"
         xmlns:RDF="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
<Seq about="NC:BookmarksTopRoot"><li><Seq about="NC:BookmarksRoot"/></li></Seq>
</RDF:RDF>
EOD;
			$xml->loadXML($out);
			$xmlRoot = $xml->getElementsByTagName('Seq')->item(0);
			$xmlParent = $xml->getElementsByTagName('Seq')->item(1);
			$parentTree = array();
			$parentSize = 0;
			$indexes = array();	//keeps indexes of folder's based on their paths
			if($row = db::fetch_row($result)){
				$cvsinPaths = array();
				//prepare indexes of all the folders that were checkedin
				if (!is_null($doc)){
					self::prepatePathIndexes($doc,$cvsinPaths);
				}
				do{
					$count = 0;
					if ($row[0] != null){
						$tree = preg_split('-/-',$row[0]);
						$tree_size = count($tree);
						foreach($parentTree as $name){
							if ($count == $tree_size || $name !== $tree[$count])
								break;
							++$count;
						}
					}else{
						$tree = array();
						$tree_size = 0;
					}
					//close previous folders
					while($count < $parentSize){
						array_pop($parentTree);
						$xmlParent = $xmlParent->parentNode->parentNode;
						--$parentSize;
					}
					while($parentSize < $tree_size){
						$parentTree[] = $tree[$parentSize];
						$impl = implode('/',$parentTree);
						$node = $xml->createElement('li');
						$xmlParent = $xmlParent->appendChild($node);
						$node = $xml->createElement('Seq');
						$xmlParent = $xmlParent->appendChild($node);
						if (isset($cvsinPaths[$impl])){
							$about = $cvsinPaths[$impl];
						}else{
							$about = 'rdf:#$'.genIDString(6);
							$node->setAttribute('NC:Name',$tree[$parentSize]);
						}
						$node->setAttribute('about',$about);
						$parentSize++;
					}
					$info = LinkManager::extractInfoArray($row[1]);
					$urlabout = 'rdf:#$'.genIDString(6);
					
					$node2 = $xml->createElement('li');
					$node = $xml->createElement('Description');
					$node2->appendChild($node);
					$xmlParent->appendChild($node2);
					$node->setAttribute('about',$urlabout);
					foreach($info as $propertyName => $value){
						$node->setAttribute(' NC:'.$propertyName,$value);
					}
				}while($row = db::fetch_row($result));
			}
			return $xmlRoot;
		}catch(Exception $e){
			error_log('failed outputing latest revision:'.$e->getLine().':'.$e->getMessage().':'.$e->getCode());
			throw $e;
		}
	}
	static $html_attr = array(
		'LastVisitDate' => 'LAST_VISIT',
		'BookmarkAddDate' => 'ADD_DATE',
		'LastModifiedDate' => 'LAST_MODIFIED',
		'FeedURL' => 'FEEDURL',
		'LastCharset' => 'LAST_CHARSET',
		'URL' => 'HREF',
		'ShortcutURL' => 'SHORTCUTURL');
	/**
	 * @param string $userid
	 * @param mysql_result $result
	 * @param xml_document $doc
	 * @return string xml output
	 */
	public function latestUserHtml($result = null){
		$userid = $this->userid;
		try{
			$sync_t = new table_synclatest();
			$cur_time = time();
			$out = <<<EOD
<!DOCTYPE NETSCAPE-Bookmark-file-1>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=UTF-8">
<TITLE>Bookmarks</TITLE>
<H1 LAST_MODIFIED="$cur_time">Bookmarks</H1>
<DL><p/>

EOD;
			$parentTree = array();
			$parentSize = 0;
			$indexes = array();	//keeps indexes of folder's based on their paths
			if($row = db::fetch_row($result)){
				//$cvsinPaths = array();
				do{
					$info = LinkManager::extractInfoArray($row[1]);
					//append each one of the properties
					$urlv = $info['Name'];
					unset($info['Name']);
					$description = '';
					if (isset($info['Description'])){
						$description = '<DD>'.htmlentities($info['Description'],ENT_COMPAT,'UTF-8')."\n";
						unset($info['Description']);
					}
					$urlabout = 'rdf:#$'.genIDString(6);
					$url = '<DT><A ICON="data:" ID="'.$urlabout.'"';
					foreach($info as $propertyName => $value){
						$url .= ' '.self::$html_attr[$propertyName].'="'
							.htmlentities($value,ENT_COMPAT,'UTF-8').'"';
					}
					$url .= ">".htmlentities($urlv,ENT_COMPAT,'UTF-8')."</A>\n";
					$count = 0;
					if ($row[0] != null){
						$tree = preg_split('-/-',$row[0]);
						$tree_size = count($tree);
						foreach($parentTree as $name){
							if ($count == $tree_size || $name !== $tree[$count])
								break;
							++$count;
						}
					}else{
						$tree = array();
						$tree_size = 0;
					}
					//close previous folders
					while($count < $parentSize){
						array_pop($parentTree);
						$out .= str_repeat("\t",$parentSize+1)."</DL><p/>\n";
						--$parentSize;
					}
					while($parentSize < $tree_size){
						$parentTree[] = $tree[$parentSize];
						$impl = implode('/',$parentTree);
						$tabs = str_repeat("\t",$parentSize+1);
						/*if (isset($cvsinPaths[$impl])){
							$about = $cvsinPaths[$impl];
							$out .= $tabs.'<DT><H3 ID="'.$about."\"/>\n$tabs<DL><p/>\n";
						}else{*/
							$about = 'rdf:#$'.genIDString(6);
							$out .= $tabs.'<DT><H3 ID="'.$about.'">'
								.htmlentities($tree[$parentSize],ENT_COMPAT,'UTF-8')."</H3>\n$tabs<DL><p/>\n";
						//}
						$parentSize++;
					}
					$out .= str_repeat("\t",$parentSize+1).$url
						.$description;
				}while($row = db::fetch_row($result));
			}
			//unset($cvsinPaths);
			if (!empty($count)){
				while($count < $parentSize){
					$out .= str_repeat("\t",$parentSize)."</DL><p/>\n";
					--$parentSize;
				}
			}
			$out .= '</DL><p/>';
			return $out;
		}catch(Exception $e){
			error_log('failed outputing latest revision:'.$e->getLine().':'.$e->getMessage().':'.$e->getCode());
			throw $e;
		}
	}
	private static function prepatePathIndexes(&$doc,&$cvsinPaths){
		$nodes = $doc->getElementsByTagNameNS(self::rdfRdf,'Seq');
		$node_length = $nodes->length;
		for ($node_count=0;$node_count<$node_length;$node_count++){
			$node = $nodes->item($node_count);
			
			$chain = array();
			$firstAbout = false;
			while($node != null 
					&& (($about = $node->getAttributeNS(self::rdfRdf,'about')) 
							!= 'NC:BookmarksRoot')){
				//only save about of the bottom child
				if ($firstAbout === false)$firstAbout = $about;
				$name = $node->getAttributeNS(self::rdfRdf,'Name');
				array_unshift($chain,$name);
				$node = $node->parentNode;
			}
			if ($firstAbout !== false){
				$s = implode('/',$chain)."\r\n";
				$cvsinPaths[implode('/',$chain)] = $firstAbout;
			}
		}
	}
}
?>