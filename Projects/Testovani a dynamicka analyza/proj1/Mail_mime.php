<?php
/* Pripraveno pro test doubles. */
/* Primo vyzadovano testovanym souborem. */

class Mail_mime
{	
	public static $body;
	
	public function setHTMLBody($body)
    {
		Mail_mime::$body = $body;
		return;
	}
	
	public function get()
    {
		return Mail_mime::$body;
	}
	
	public function headers($headers)
    {
		return $headers;
	}
	
	
}


?>
