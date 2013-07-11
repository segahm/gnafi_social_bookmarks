<?php
class HtmlTable{
	/** @var current row count*/
	private $row = 0;
	/** @var current collumn count*/
	private $col = 0;
	private $colSize = 0;
	private $auto_grow = true;
	private $buffer = '';
	private $autoFill = '&nbsp;';
	private $rowAttributes = '';
	private $rowAttributesSize = -1;
	private $colAttributes = '';
	private $colAttributesSize = -1;
	private $tableAttr = '';
	private $type;
	const TYPE_BYROW = 1;	//1st - 1st row, 2nd element - 2nd row
	const TYPE_BYCOL = 2;	//1st - 1st row, 2nd element - 2nd col
	private $cells = array();
	public $onEmptyInsert = '<tr><td>&nbsp;</td></tr>';
	/**
	 * @param int $size column size
	 * @param string $tableAttr tables attributes
	 * @param mixed $rowAttributes string to insert for every row or an array with alternate values 
	 * @param mixed $colAttributes string or an array to apply to corresponding columns
	 * @param int $typeOfTable wherever to alling values by collumn or by row
	 *(ex: <tr $rowAttributes> or <tr $rowAttributes[0]>...<tr $rowAttributes[1])
	 */
	function HtmlTable($size = 0,$tableAttr = '',$rowAttributes = '',$colAttributes = '',
			$typeOfTable = self::TYPE_BYCOL){
		assert(is_numeric($size) 
			&& (is_string($tableAttr)) 
			&& (is_string($rowAttributes) || is_array($rowAttributes))
			&& (is_string($colAttributes) || is_array($colAttributes)));
		$this->colSize = $size;
		$this->tableAttr = $tableAttr;
		$this->rowAttributes = $rowAttributes;
		if (is_array($rowAttributes))
			$this->rowAttributesSize = count($rowAttributes);
		$this->colAttributes = $colAttributes;
		if (is_array($colAttributes))
			$this->colAttributesSize = count($colAttributes);
		$this->type = $typeOfTable;
	}
	/**
	 * @param int $size new number of collumns
	 */
	private function _adjustSize($size){
		assert(is_numeric($size));
		if ($this->auto_grow && $row <= 1){
			$this->colSize = $size;
			return true;
		}else{
			return false;
		}
	}
	/**
	 * @param string $value value to insert inside of the td cell
	 * @param string $attr string that overwrites default td attributes(i.e. bgcolor="" style=""...)
	 */
	public function addCell($value = null,$attr = null){
		if (is_null($value))
			$value = $this->autoFill;
		if ($this->type == self::TYPE_BYCOL){
			if ($this->col == 0){
				$this->addNewRow(array($value),$attr);
			}else{
				$this->_insureColAttributes($attr);
				$this->buffer .= "<td".((!empty($attr[$this->col]))?' '.$attr[$this->col]:'').">$value</td>";
				$this->col++;
				if ($this->col == $this->colSize){
					$this->col = 0;
					$this->buffer .= '</tr>';
				}
			}
		}else{
			$this->cells[] = array($value,$attr);
		}
	}
	private function _insureColAttributes(&$temp = null){
		if (is_null($temp)){
			$temp = $this->colAttributes;
			$tempSize = $this->colAttributesSize;
		}elseif(is_array($temp)){
			$tempSize = count($temp);
			//set default values for the rest of the unspecified indexes
			for ($i=$tempSize;$i<$this->colAttributesSize;$i++){
				$temp[] = $this->colAttributes[$i];
			}
		}else{
			$tempSize = -1;
		}
		//if we have just one default value, then set indexes to point to
		if ($tempSize == -1){
			$str = $temp;
			$temp = array();
			if(!empty($str)){
				for($i=0;$i<$this->colSize;$i++){
					$temp[] = $str; 
				}
			}
		}
	}
	private function _insureRowAttribute(&$rowAttr = null){
		if (is_null($rowAttr)){
			$rowAttr = $this->rowAttributes;
			$rowAttrSize = $this->rowAttributesSize;
		}
		$strAttr = '';
		if (!empty($rowAttr))
			$strAttr = ' ';
		if (is_string($rowAttr)){
			$strAttr .= $rowAttr;
		}elseif ($this->row < $rowAttrSize){
			$strAttr .= $rowAttr[$this->row];
		}else{
			$strAttr .= $rowAttr[$this->row%$rowAttrSize];
		}
		$rowAttr = $strAttr;
	}
	/**
	 * @param array $value array('value1','value2')
	 * @param mixed $colAttr either a string of attributes applied to all or an array('attributes1','attributes2')
	 * @param string $rowAttr overwrites default row attributes
	 */
	public function addNewRow($value,$colAttr = null,$rowAttr = null){
		assert($this->type == self::TYPE_BYCOL);
		assert(is_array($value));
		$valSize = count($value);
		//if size is not yet set, then set it
		if ($this->row == 0 && $this->colSize == 0)
			$this->colSize = $valSize;
		elseif($this->col != 0){
			$this->_finishLastRow();
		}
		assert($valSize <= $this->colSize);
		$this->_insureRowAttribute($rowAttr);
		$this->row++;
		$this->_insureColAttributes($colAttr);
		$buffer = "<tr$rowAttr>";
		
		for(;$this->col<$this->colSize && isset($value[$this->col]);$this->col++){
			$strAttr = '';
			if (!empty($colAttr[$this->col]))
				$strAttr = " {$colAttr[$this->col]}";
			$buffer .= "<td$strAttr>{$value[$this->col]}</td>";
		}
		if ($this->col == $this->colSize){
			$buffer .= '</tr>';
			$this->col = 0;
		}
		$this->buffer .= $buffer;
	}
	private function _finishLastRow(){
		$col = $this->col;
		if ($col != 0){
			while($this->col != 0){
				$this->addCell();
			}
		}
	}
	/**
	 *returns a reference to the finalized buffered table content*/
	public function &flush(){
		if ($this->type == self::TYPE_BYCOL){
			$this->_finishLastRow();
			$this->buffer = '<table'.((!empty($this->tableAttr))?" $this->tableAttr":'').'>'.$this->buffer.'</table>';
			if (empty($this->cells)){
				if (!is_null($this->onEmptyInsert)){
					$this->buffer = '<table'.((!empty($this->tableAttr))?" $this->tableAttr":'').'>'
						.$this->onEmptyInsert.'</table>';
				}else{
					$this->buffer = '';
				}
			}
		}else{
			$this->buffer = '<table'.((!empty($this->tableAttr))?" $this->tableAttr":'').'>';
			if (empty($this->cells)){
				if (!is_null($this->onEmptyInsert)){
					$this->buffer .= $this->onEmptyInsert;
				}else{
					$this->buffer = '';
					return '';
				}
			}
			$this->_finishLastRow();
			$total = count($this->cells);
			$colSize =& $this->colSize;
			//finish line
			$remainder = ($total>$colSize)?$colSize-$total%$colSize:$colSize-$total;
			for($i=0;$i<$remainder;$i++){
				$this->addCell();
			}
			$total += $remainder;
			$cells =& $this->cells;
			$eachLength = $total/$colSize;
			for($key=0;$key<$eachLength;$key++){
				$this->row++;
				$attr = null;
				$this->_insureRowAttribute($attr);
				$this->buffer .= "<tr$attr>";
				for($this->col=0;$this->col<$colSize;$this->col++){
					$attr = $cells[$key+($eachLength*$this->col)][1];
					$this->_insureColAttributes($attr);
					$this->buffer .= '<td'.(!empty($attr[$this->col])?" {$attr[$this->col]}":'').'>'
						.$cells[$key+($eachLength*$this->col)][0].'</td>';
				}
				$this->buffer .= '</tr>';
			}
			$this->buffer .= '</table>';
			unset($this->cells);
		}
		return $this->buffer;
	}
}
?>