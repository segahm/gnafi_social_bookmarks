<?php
class parser{
	public $form;
	function parser(&$array){
		$this->form = $array;
	}
	function isDefined($element){
		return isset($this->form[$element]);
	}
	function getString($element, $default = null){
		if (isset($this->form[$element]) && !is_null($element) && trim($this->form[$element]) != ''){
			return trim($this->form[$element]);
		}else{
			return $default;
		}
	}
	function getDate($element, $default = null){
		if (isset($this->form[$element]) 
			&& !empty($this->form[$element]) && !ereg('^[0\-]+$',$this->form[$element])){
			return trim($this->form[$element]);
		}else{
			return $default;
		}
	}
	function getEmail($element,$default){
		$str = $this->getString($element);
		if (is_null($str) || !Validate::email($str)){
			return $default;
		}
		return $str;
	}
	function getPassword($element,$default){
		$str = $this->getString($element);
		if (is_null($str) || !funcs_isValidPass($str)){
			return $default;
		}
		return $str;
	}
	function getHTML($element, $default = ''){
		if (isset($this->form[$element]) && trim($this->form[$element]) != ''){
			return toHTML(trim($this->form[$element]));
		}else{
			return $default;
		}
	}
	function getRadio($element, $value){
		if (isset($this->form[$element]) && $this->form[$element] == $value){
			return 'checked';
		}else{
			return '';
		}
	}
	function getSelect($element, $value){
		if (isset($this->form[$element]) && $this->form[$element] == $value){
			return 'selected';
		}else{
			return '';
		}
	}
	function getSql($element, $default = null){
		return toSQL($this->getString($element,$default));
	}
	function getInt($element, $default = null){
		if (isset($this->form[$element])
			&& is_numeric(trim($this->form[$element]))){
			return (int)trim($this->form[$element]);
		}else{
			return $default;
		}
	}
	function getIntP($element, $default = null){
		$el = $this->getInt($element,-1);
		if ($el <= 0)
			return $default;
		else
			return $el;
	}
	function getIntN($element, $default = null){
		$el = $this->getInt($element,-1);
		if ($el < 0)
			return $default;
		else
			return $el;
	}
	/**
	 * Just an alias for getIntN but with default value set to 0
	 */
	static $lastSqlStart = null;
	function getSqlStart($name,$default = 0){
		$element = $this->getIntN($name,$default);
		self::$lastSqlStart = array($element,$name,$_SERVER['REQUEST_URI']);
		return $element;
		
	}
	/**
	 * This should be used to get values to be applied to sql limit
	 */
	static $lastSqlLimit = null;
	function getSqlLimit($name,$default = 20,$max = 1000,$min = 1){
		assert($default >= $min && $default <= $max);
		$element = $this->getIntP($name,null);
		if (is_null($element) || $element > $max || $element < $min)
			$element = $default;
		self::$lastSqlLimit = array($element,$name,$_SERVER['REQUEST_URI']);
		return $element;
	}
	function getSqlPages($curResCount,$totalResults = null){
		assert(self::$lastSqlStart != null && self::$lastSqlLimit != null);
		$url = $_SERVER['REQUEST_URI'];
		assert(self::$lastSqlLimit[2] === $url && self::$lastSqlStart[2] === $url);
		$str = '';
		$url = preg_replace('/[&]?'.self::$lastSqlStart[1].'=[^&]+/i','',$url);
		$match = array();
		if (!preg_match('-\?(.*)$-',$url,$match))
			$url .= '?';
		$url .= '&'.self::$lastSqlStart[1].'=';
		if (self::$lastSqlStart[0] > 0){
			$str .= '<div class="sql-p-d-l"><a class="sql-s" href="'.$url
				.abs((self::$lastSqlStart[0]-self::$lastSqlLimit[0]+(is_null($totalResults)?1:0)))
				.'">previous</a></div>';
		}
		if (!is_null($totalResults)){
			
		}
		if ($curResCount == self::$lastSqlLimit[0]){
			$str .= '<div class="sql-p-d-r"><a class="sql-l" href="'.$url
				.(self::$lastSqlStart[0]+self::$lastSqlLimit[0]-(is_null($totalResults)?1:0))
				.'">next</a></div>';
		}
		return $str;
	}
	/**
	 * @param string $elementName name of the element
	 * @param int $start start parameter
	 * @param int $limit by how much to shift the value to the left (start=max(0,$start-$limit))
	 */
	function getSqlStartShiftedLeft($elementName,$start = null,$limit = null){
		if (is_null($start))
			$start = $this->getSqlStart($elementName);
		if (is_null($limit))
			$limit = $this->getSqlLimit($elementName);
		$url = preg_replace('/[\&]?'.$startStr.'=[^&]+/i','',$url);
		if (strpos($url,'?') === false)
			$url .= '?';
		else
			$url .= '&';
		$url .= $startStr.'='.max(0,$start-$limit);
	}
	/**
	 * @param string $elementName name of the element
	 * @param int $start start parameter
	 * @param int $limit by how much to shift the value to the left (start=start+limit-1)
	 */
	function getSqlStartShiftedRight($elementName,$start = null,$limit = null){
		if (is_null($start))
			$start = $this->getSqlStart($elementName);
		if (is_null($limit))
			$limit = $this->getSqlLimit($elementName);
		$url = preg_replace('/[\&]?'.$startStr.'=[^&]+/i','',$url);
		if (strpos($url,'?') === false)
			$url .= '?';
		else
			$url .= '&';
		$url .= $startStr.'='.max($start+$limit-1);
	}
	function getBoolean($element, $default = null){
		if (isset($this->form[$element])
			&& is_bool(trim($this->form[$element]))){
			return (bool)trim($this->form[$element]);
		}else{
			return $default;
		}
	}
}
?>