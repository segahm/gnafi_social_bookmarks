<?php
header('Content-type: application/xml; charset=UTF-8');
$xml = new DOMDocument('1.0','UTF-8');
$link = toHTML('http://gnafi.com'.$_SERVER['REQUEST_URI']);
$str = <<<EOD
	<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
  			xmlns="http://purl.org/rss/1.0/"
  			xmlns:gn="http://gnafi.com#">
	<channel rdf:about="$link">
    <title>Gnafi.com</title>
    <link>$link</link>
    <description>Gnafi.com allows you keep your bookmarks synchronized.</description>
    <language>en-us</language>
    <items>
      <rdf:Seq/>
    </items>
  </channel>
  </rdf:RDF>
EOD;
$xml->loadXML($str);
$rdf = $xml->getElementsByTagNameNS('http://www.w3.org/1999/02/22-rdf-syntax-ns#','RDF')->item(0);
$itemsSeq = $xml->getElementsByTagNameNS('http://www.w3.org/1999/02/22-rdf-syntax-ns#','Seq')->item(0);
if ($resultSet != null && $row = db::fetch_row($resultSet)){
	assert(isset($field_keys));
	assert(isset($url_t));
	assert(isset($users_t));
	assert(isset($parser));
	do{
		$e = $xml->createElement('rdf:li');
		$e->setAttribute('rdf:resource',$row[$field_keys[$url_t->field_url]]);
		$itemsSeq->appendChild($e);
		
		$item = $xml->createElement('item');
		$item->setAttribute('rdf:about',$row[$field_keys[$url_t->field_url]]);
		$rdf->appendChild($item);
		
		$e = $xml->createElement('title');
		$item->appendChild($e);
		$e->appendChild($xml->createTextNode($row[$field_keys[$url_t->field_title]]));
		$e = $xml->createElement('link');
		$e->appendChild($xml->createTextNode($row[$field_keys[$url_t->field_url]]));
		$item->appendChild($e);
		
		if (!empty($row[$field_keys[$url_t->field_keywords]])){
			$tags = CatPath::explodeStrToKeywords($row[$field_keys[$url_t->field_keywords]]);
			$e = $xml->createElement('gn:tags');
			$item->appendChild($e);
			$e = $e->appendChild($xml->createElement('rdf:Seq'));
			foreach($tags as $tag){
				$e->appendChild($xml->createElement('rdf:li',$tag));
			}
		}
		if (isset($field_keys[$users_t->field_username])){
			$e = $xml->createElement('gn:user',$row[$field_keys[$users_t->field_username]]);
			$item->appendChild($e);
		}
		if (!empty($row[$field_keys[$url_t->field_description]])){
			$e = $xml->createElement('gn:description');
			$e->appendChild($xml->createTextNode($row[$field_keys[$url_t->field_description]]));
			$item->appendChild($e);
		}
		$e = $xml->createElement('gn:date',$row[$field_keys[$url_t->field_time]]);
		$item->appendChild($e);
	}while($row = db::fetch_row($resultSet));
}
echo $xml->saveXML();
?>