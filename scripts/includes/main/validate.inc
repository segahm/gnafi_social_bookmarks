<?php
/**
 * Methods for common data validations
 */
define('VALIDATE_NUM',          '0-9');
define('VALIDATE_SPACE',        '\s');
define('VALIDATE_ALPHA_LOWER',  'a-z');
define('VALIDATE_ALPHA_UPPER',  'A-Z');
define('VALIDATE_ALPHA',        VALIDATE_ALPHA_LOWER . VALIDATE_ALPHA_UPPER);
define('VALIDATE_EALPHA_LOWER', VALIDATE_ALPHA_LOWER . 'áéíóúàèìòùäëïöüâêîôûñçþæð');
define('VALIDATE_EALPHA_UPPER', VALIDATE_ALPHA_UPPER . 'ÁÉÍÓÚÀÈÌÒÙÄËÏÖÜÂÊÎÔÛÑÇÞÆÐ');
define('VALIDATE_EALPHA',       VALIDATE_EALPHA_LOWER . VALIDATE_EALPHA_UPPER);
define('VALIDATE_PUNCTUATION',  VALIDATE_SPACE . '\.,;\:&"\'\?\!\(\)');
define('VALIDATE_NAME',         VALIDATE_EALPHA . VALIDATE_SPACE . "'");
define('VALIDATE_STREET',       VALIDATE_NAME . "/\\ºª");

/**
 * Validation class
 *
 * Package to validate various datas. It includes :
 *   - numbers (min/max, decimal or not)
 *   - email (syntax, domain check)
 *   - string (predifined type alpha upper and/or lowercase, numeric,...)
 *   - date (min, max)
 *   - uri (RFC2396)
 *   - possibility valid multiple data with a single method call (::multiple)
 */
class Validate
{
    /**
     * Validate a number
     *
     * @param string    $number     Number to validate
     * @param array     $options    array where:
     *                              'decimal'   is the decimal char or false when decimal not allowed
     *                                          i.e. ',.' to allow both ',' and '.'
     *                              'dec_prec'  Number of allowed decimals
     *                              'min'       minimun value
     *                              'max'       maximum value
     *
     * @return boolean true if valid number, false if not
     *
     * @access public
     */
    public static function number($number, $options = array())
    {
        $decimal = $dec_prec = $min = $max = null;
        if (is_array($options)) {
            extract($options);
        }

        $dec_prec   = $dec_prec ? "{1,$dec_prec}" : '+';
        $dec_regex  = $decimal  ? "[$decimal][0-9]$dec_prec" : '';

        if (!preg_match("|^[-+]?\s*[0-9]+($dec_regex)?\$|", $number)) {
            return false;
        }

        if ($decimal != '.') {
            $number = strtr($number, $decimal, '.');
        }

        $number = (float)str_replace(' ', '', $number);
        if ($min !== null && $min > $number) {
            return false;
        }

        if ($max !== null && $max < $number) {
            return false;
        }
        return true;
    }

    /**
     * Validate a email
     *
     * @param string    $email          URL to validate
     * @param boolean   $check_domain   Check or not if the domain exists
     *
     * @return boolean true if valid email, false if not
     *
     * @access public
     */
    public static function email($email, $check_domain = false)
    {
        // partially "Borrowed" from PEAR::HTML_QuickForm and refactored
        $regex = '&^(?:                                               # recipient:
         ("\s*(?:[^"\f\n\r\t\v\b\s]+\s*)+")|                          #1 quoted name
         ([-\w!\#\$%\&\'*+~/^`|{}]+(?:\.[-\w!\#\$%\&\'*+~/^`|{}]+)*)) #2 OR dot-atom
         @(((\[)?                     #3 domain, 4 as IPv4, 5 optionally bracketed
         (?:(?:(?:(?:25[0-5])|(?:2[0-4][0-9])|(?:[0-1]?[0-9]?[0-9]))\.){3}
               (?:(?:25[0-5])|(?:2[0-4][0-9])|(?:[0-1]?[0-9]?[0-9]))))(?(5)\])|
         ((?:[a-z0-9](?:[-a-z0-9]*[a-z0-9])?\.)*[a-z](?:[-a-z0-9]*[a-z0-9])?))  #6 domain as hostname
         $&xi';
        if (preg_match($regex, $email)) {
            if ($check_domain && function_exists('checkdnsrr')) {
                list (, $domain)  = explode('@', $email);
                if (checkdnsrr($domain, 'MX') || checkdnsrr($domain, 'A')) {
                    return true;
                }
                return false;
            }
            return true;
        }
        return false;
    }

    /**
     * Validate a string using the given format 'format'
     *
     * @param string    $string     String to validate
     * @param array     $options    Options array where:
     *                              'format' is the format of the string
     *                                  Ex: VALIDATE_NUM . VALIDATE_ALPHA (see constants)
     *                              'min_length' minimum length
     *                              'max_length' maximum length
     *
     * @return boolean true if valid string, false if not
     *
     * @access public
     */
    public static function string($string, $options)
    {
        $format = null;
        $min_length = $max_length = 0;
        if (is_array($options)) {
            extract($options);
        }
        if ($format && !preg_match("|^[$format]*\$|s", $string)) {
            return false;
        }
        if ($min_length && strlen($string) < $min_length) {
            return false;
        }
        if ($max_length && strlen($string) > $max_length) {
            return false;
        }
        return true;
    }

    /**
     * Validate an URI (RFC2396)
     * This function will validate 'foobarstring' by default, to get it to validate
     * only http, https, ftp and such you have to pass it in the allowed_schemes
     * option, like this:
     * <code>
     * $options = array('allowed_schemes' => array('http', 'https', 'ftp'))
     * var_dump(Validate::uri('http://www.example.org'), $options);
     * </code>
     *
     * @param string    $url        URI to validate
     * @param array     $options    Options used by the validation method.
     *                              key => type
     *                              'domain_check' => boolean
     *                                  Whether to check the DNS entry or not
     *                              'allowed_schemes' => array, list of protocols
     *                                  List of allowed schemes ('http',
     *                                  'ssh+svn', 'mms')
     *
     * @return boolean true if valid uri, false if not
     *
     * @access public
     */
    public static function uri($url, $options = null)
    {
        $strict = ';/?:@$,';
        $domain_check = false;
        $allowed_schemes = null;
        if (is_array($options)) {
            extract($options);
        }
        if (preg_match(
             '&^(?:([a-z][-+.a-z0-9]*):)?                             # 1. scheme
              (?://                                                   # authority start
              (?:((?:%[0-9a-f]{2}|[-a-z0-9_.!~*\'();:\&=+$,])*)@)?    # 2. authority-userinfo
              (?:((?:[a-z0-9](?:[-a-z0-9]*[a-z0-9])?\.)*[a-z](?:[-a-z0-9]*[a-z0-9])?\.?)  # 3. authority-hostname OR
              |([0-9]{1,3}(?:\.[0-9]{1,3}){3}))                       # 4. authority-ipv4
              (?::([0-9]*))?)?                                        # 5. authority-port
              ((?:/(?:%[0-9a-f]{2}|[-a-z0-9_.!~*\'():@\&=+$,;])+)+/?)? # 6. path
              (?:\?([^#]*))?                                          # 7. query
              (?:\#((?:%[0-9a-f]{2}|[-a-z0-9_.!~*\'();/?:@\&=+$,])*))? # 8. fragment
              $&xi', $url, $matches)) {
            $scheme = isset($matches[1]) ? $matches[1] : '';
            $authority = isset($matches[3]) ? $matches[3] : '' ;
            if (is_array($allowed_schemes) &&
                !in_array($scheme,$allowed_schemes)
            ) {
                return false;
            }
            if (isset($matches[4])) {
                $parts = explode('.', $matches[4]);
                foreach ($parts as $part) {
                    if ($part > 255) {
                        return false;
                    }
                }
            } elseif ($domain_check && function_exists('checkdnsrr')) {
                if (!checkdnsrr($authority, 'A')) {
                    return false;
                }
            }
            if ($strict) {
                $strict = '#[' . preg_quote($strict, '#') . ']#';
                if ((isset($matches[7]) && preg_match($strict, $matches[7]))
                 || (isset($matches[8]) && preg_match($strict, $matches[8]))) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    public static function html(&$html,$length = null){
		$error = true;
    	$dom = new DOMDocument();
		//init dom properties
		$dom->encoding = 'iso-8859-1';
		$dom->resolveExternals = false;
		$dom->formatOutput = true;
		$dom->preserveWhiteSpace = false;
		$dom->validateOnParse = false;
		//set error handler to record all the errors while parsing
		set_error_handler(
			create_function('$er_code,$error,$errfile, $errline',
				'$GLOBALS["MY_DOM_ERROR"] = $error;'
			)
		);
		$dom->loadHTML($html);
		if (isset($GLOBALS['MY_DOM_ERROR'])){
			echo $GLOBALS['MY_DOM_ERROR'];
			$error = $GLOBALS['MY_DOM_ERROR'];
			//format error to be user friendly
			$indx = stripos($error,']:');
			if ($indx === false){
				$error = false;
			}else{
				$error = substr($error,$indx+2);
			}
		}else{
			$dom->normalize();
			restore_error_handler();
			$html =& $dom->saveHTML();
		}
		return $error;
    }
    /**
     * @param text $text template text to   validate 
     * @return boolean true if valid text, text error if invalid
     *
     * @access public
     */
    public static function template(&$text){
    	try{
    		return true;
    		/*//first validate html
    		$dtdStr = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">';
    		$dom = new DOMDocument();
			//init dom properties
			$dom->encoding = 'iso-8859-1';
			$dom->resolveExternals = false;
			$dom->formatOutput = true;
			$dom->preserveWhiteSpace = false;
			$dom->validateOnParse = false;
			$dom->strictErrorChecking = false;
			//set error handler to record all the errors while parsing
			set_error_handler(
				create_function('$er_code,$error,$errfile, $errline',
					'$GLOBALS["MY_DOM_ERROR"] = $error;'
				)
			);
			$dom->loadHTML($text);
			if (isset($GLOBALS['MY_DOM_ERROR'])){
				$error = $GLOBALS['MY_DOM_ERROR'];
				//format error to be user friendly
				$indx = stripos($error,']:');
				if ($indx !== false){
					return substr($error,$indx+2);
				}else{
					return false;	//we don't know what the actual error is
				}
			}
			$dom->normalize();
			restore_error_handler();
			$text =& $dom->saveHTML();
			//DOMNodeList
			$tags = $dom->getElementsByTagName('img');
			for($i=0;$i<$tags->length;$i++){
				//DOMNode
				$item = $tags->item($i);
				//DOMNamedNodeMap 
				$atrs = $item->attributes;
				if (!is_null($atrs)){
					//DOMNode
					$atr = $atrs->getNamedItem('src');
					if (is_null($atr))continue;
					$value = $atr->value;
					$matches = array();
					//now check that source code doesn't point anywhere other than /img/name
					if (!preg_match('~^/img/[a-z0-9]+$~i',$value) && !preg_match('~^http://www.gnafi.com/img/[a-z0-9]+$~i',$value)
						&& !preg_match('~^http://gnafi.com/img/[a-z0-9]+$~i',$value))
						return 'please do not use images that are located outside of our domain';
				}
			}
			$tags = $dom->getElementsByTagName('script');
			for($i=0;$i<$tags->length;$i++){
				//DOMNode
				$item = $tags->item($i);
				//DOMNamedNodeMap 
				$atrs = $item->attributes;
				if (!is_null($atrs)){
					//DOMNode
					$atr = $atrs->getNamedItem('src');
					if (is_null($atr))continue;
					$value = $atr->value;
					$matches = array();
					//now check that source code doesn't point anywhere other than /img/name
					if (preg_match('~http://([^/]+)~i',$value,$matches) 
						&& ($matches[1] != GLOBAL_DOMAIN_NAME && $matches[1] != ('www.'.GLOBAL_DOMAIN_NAME)))
						return 'please do not use scripts outside of our domain';
				}
			}*/
    	}catch(Exception $e){
    		log_writeLog($e->getMessage());
    		return false;
    	}
    	return true;
    }
    /**
     * @param text $text css text to validate 
     * @return boolean true if valid text, text error if invalid
     *
     * @access public
     */
    public static function css(&$text){
    	try{
	    	//at this point just scan to see if the any html tags are present
	    	$pos = strpos($text,'<');
	    	if (($pos = strpos($text,'<')) !== false){
	    		//check wherever this seems like a an actual html tag, but don't be too strict about it
	    		if (($spos = strpos($text,'>',$pos)) !== false){
	    			$str = substr($text,$pos,$spos-$pos+1);
	    			if (preg_match('/^\<[a-z]+.*\>$/i',$str))
	    				return 'css error: no html tags are allowed in the css part of the template';
	    		}
	    	}
	    	//now see if there are urls pointing to outside of the domain
	    	$matches = array();
	    	if (preg_match_all('/url\([ \'\"]*([^\"\'\)]+)[ \"\']*\)/i',$text,$matches,PREG_PATTERN_ORDER)){
	    		foreach($matches[1] as $match){
	    			if (preg_match('/http:\/\/(?!(?:www\.gnafi\.com)|(?:gnafi\.com))/i',$match))
	    				return 'css error('.toHTML($match).'): please do not use links that point outside of gnafi.com';
	    			if (preg_match('/^(?!http)(?!\/img\/[^\/]+\.(?:jpeg|jpg))/i',$match)
	    					|| preg_match('/http:\/\/(?!(?:www.gnafi.com\/img\/[^\/]+\.(?:jpeg|jpg))|(?:gnafi.com\/img\/[^\/]+\.(?:jpeg|jpg)))/i',
	    					$match))
	    				return 'css error('.toHTML($match).'): please resort to using "/img/image_name.jpg" for all your images';
	    		}
	    	} 
    	}catch(Exception $e){
    		log_writeLog($e->getMessage());
    		return false;
    	}
    	return true;
    }
}

?>
