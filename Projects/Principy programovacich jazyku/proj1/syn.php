<?php

#SYN:xmutna00

//Spracovanie argumentov
$shortopts = "";

$longopts  = array(
    "help",
    "format::",
    "input::",
    "output::",
    "br",
);

$options = getopt($shortopts, $longopts);

if ($argc > 5) {
	exit(1);
}

$help = 0;
$br = 0;
$input = 0;
$format = 0;
$output = 0;
$outputNotSet = false;

//Overenie legitimnosti argumentov
for ($i = 1; $i < $argc;$i++)
{
	if (preg_match('/^--help$/',$argv[$i]))
		$help++;
		else if (preg_match('/^--br$/',$argv[$i]))
			$br++;
			else if (preg_match('/^--format/',$argv[$i]))
				$format++;
				else if (preg_match('/^--input/',$argv[$i]))
					$input++;
					else if (preg_match('/^--output/',$argv[$i]))
						$output++;
						else exit(1);
	
}

if ($help > 1) exit(1);
if ($input > 1) exit(1);
if ($output > 1) exit(1);
if ($br > 1) exit(1);
if ($help > 1) exit(1);

if ($help && $argc > 2)
	exit(1);

// Otvorenie suborou
if ($input && $help == 0) {	
	$inputFile = @file_get_contents($options['input'], FILE_USE_INCLUDE_PATH);
	if ($inputFile === FALSE)
		exit(2);
} else if (!$input && $help == 0)
	$inputFile = @file_get_contents('php://stdin', FILE_USE_INCLUDE_PATH);

if ($output && $help == 0){
	if (file_exists($options['output'] == false))
		$outputNotSet = true;
		
	if ($outputNotSet == false)
	{
		if (is_writable($options['output'] == false))
			exit(3);
	}
		
	$outputFile = @fopen($options['output'],"w", FILE_USE_INCLUDE_PATH);
	if ($outputFile === FALSE)
		exit(3);
}

if ($help)
{
	fwrite(STDOUT, "\n Program: SYN\n Autor: Filip Mutnansky\n\n Program zvyrazni casti textu zadane regularnym vyrazom 
	HTML elementami zadanymi formatovacimi prikazmi\n Parametre programu: \n --input=filename - urci vstupny subor (ak nie je zadany, vstup je stdin)\n --output=filename - urci vystupny subor (ak nie je zadany, vystup je stdout)\n --format=filename - urci formatovaci subor \n --br - prida na koniec kazdeho riadku <br />\n\n");
	exit(0);
}
	
if ($format)	
	$formatFile = @file_get_contents($options['format'], FILE_USE_INCLUDE_PATH);

if ($formatFile === FALSE || $format == false || strlen($formatFile) == 0) {
	if ($br) {
		$inputFile = preg_replace('/\n/','<br />'.PHP_EOL,$inputFile);
		file_put_contents($options['output'], $inputFile);
	}
	
    exit(0);
}

if (substr($formatFile, -1) !=PHP_EOL)
{
	$formatFile = substr_replace($formatFile, PHP_EOL,strlen($formatFile), 0);
}

$stack = array('$');
$regexpArray = array();
$regexp = array();
$formatexp = array();
$formatexpArray = array();
$formatexpArrayOfArrays = array();
$inRegexp = true;
$inFormatexp = false;
$separator = false;
$expecting = false;
$before = true;

//SPRACOVANIE SUBORU
$i = 0;
	
while ($i < strlen($formatFile)) {
	
	$char = $formatFile[$i];
	
	//Ak sme za ciarkou a nasleduje nebiely znak
	if ($separator == true && $inRegexp == false && $inFormatexp == false && $expecting == false && ($char != ' ' && $char != "\t"))
	{
		$inFormatexp = true;
		$separator = false;
	}
		
	// Ak ocakavame separator a pride znak rozny od separatora
	if ($inFormatexp == false && $inRegexp == false && $separator == false && $expecting == true && $before == false &&($char != ',' && $char != "\n" && $char != ' ' && $char != "\t"))
		exit(4);
	
	// Ak pocas nacitavania formatovacieho prikazu narazi na biely znak ocakava oddelovac
	if ($inFormatexp == true && ($char == ' ' || $char == "\t") && $before == false )
	{
		$inFormatexp = false;
		$expecting = true;
	}
	
	// Ak narazi na tabulator prepne do rezimu formatexp a regexp prida do pola		
	if ($char == "\t" && $inRegexp == true) {
		$string = implode ($regexp);
		array_push($regexpArray, $string);
		unset($regexp);
		$regexp = array();
		$inRegexp = false;
		$inFormatexp = true;
		$expecting = false;
		$before = true;
	}
		
	// Ak narazi na oddelovac prida string do pola formatovacich prikazov		
	if ($char == ',' || $char == "\n") {
		$string = implode ($formatexp);
		//Kontrola validity
				
		if (!preg_match('/^italic$/',$string) && !preg_match('/^bold$/',$string) && !preg_match('/^underline$/',$string) && !preg_match('/^teletype$/',$string) && !preg_match('/^size:[1-7]$/',$string) && !preg_match('/^color:([a-fA-F]|[0-9]){6}$/',$string)) 
		{
			exit(4);
		}
							
		array_push($formatexpArray,$string);
		unset($formatexp);
		$formatexp = array();
		$inFormatexp = false;
		$separator = true;
		$expecting = false;
	}
	
	// Prida do pola pole formatovacich prikazov
	if ($char == "\n") { 
		array_push($formatexpArrayOfArrays,$formatexpArray);
		unset($formatexpArray);
		$formatexpArray = array();
		$inRegexp = true;
		$inFormatexp = false;
		$separator = false;
	}
		
	// Nacitavanie po znaku
	if ($inRegexp == true && $separator == false && $char != "\n") array_push($regexp,$char);
	if ($inFormatexp == true && $separator == false && $char != "\t") 
	{
		$before = false;
		array_push($formatexp,$char);
	}
			
	$i++;
	
}

//KONECNY AUTOMAT
$state = "start";
$i = 0;
$j = 0;
$phpRegex = array();
$phpRegexArray = array();

$regexpArray = array_filter($regexpArray);
$regexpArray = array_slice($regexpArray, 0);

while ($i < count($regexpArray))
{
	$state = "start";
	while($j < strlen($regexpArray[$i]))
	{
		switch ($state) {
/*START*/		case "start":    
				
				if ($regexpArray[$i][$j] == '(')
				{
					array_push($phpRegex, $regexpArray[$i][$j]);
					array_push($stack, $regexpArray[$i][$j]);
					$state = "start";
				}	else if ($regexpArray[$i][$j] == '!')
					{
						$state = "Negation";
					}
					else if ($regexpArray[$i][$j] == '%')
					{
						$state = "specialExpression";
					}	else if ($regexpArray[$i][$j] == '.' || $regexpArray[$i][$j] == '*' || $regexpArray[$i][$j] == '+' || $regexpArray[$i][$j] == '|' || $regexpArray[$i][$j] == ')')
						$state = "error";
						else if (ord($regexpArray[$i][$j]) < 32)
							exit(4);
							else
							{
								if ($regexpArray[$i][$j] == '[' || $regexpArray[$i][$j] == ']' || $regexpArray[$i][$j] == '^' || $regexpArray[$i][$j] == '$' || $regexpArray[$i][$j] == '{' || $regexpArray[$i][$j] == '}' || $regexpArray[$i][$j] == '?' || $regexpArray[$i][$j] == '/' || $regexpArray[$i][$j] == '\\')
									array_push($phpRegex,'\\');
								
								array_push($phpRegex, $regexpArray[$i][$j]);
								$state = "general";
							}
				break;
			
/*ERROR*/	case "error":
				exit(4);
				break;
					
/*GENERAL*/	case "general":
				if ($regexpArray[$i][$j] == '(')
				{
					array_push($phpRegex, $regexpArray[$i][$j]);
					array_push($stack, $regexpArray[$i][$j]);
					$state = "start";
				}	else if ($regexpArray[$i][$j] == '.')
					$state = "start";
						else if ($regexpArray[$i][$j] == '|')
						{
							array_push($phpRegex, $regexpArray[$i][$j]);
							$state = "OperatorOR";
						}
							else if ($regexpArray[$i][$j] == '*' || $regexpArray[$i][$j] == '+')
							{	
								array_push($phpRegex, $regexpArray[$i][$j]);
								$state = "OperatorITERATE";
							}	else if ($regexpArray[$i][$j] == '%')
								{
									$state = "specialExpression";
								} else if ($regexpArray[$i][$j] == '!')
									{
										$state = "Negation";
									} 
									else if ($regexpArray[$i][$j] == ')')	
									{
										array_push($phpRegex, $regexpArray[$i][$j]);
										array_push($stack, $regexpArray[$i][$j]);
										$state = "general";
										if (end($stack) == ')' && prev($stack) == '(') {
											array_pop($stack);
											array_pop($stack);
										}	
									} else if (ord($regexpArray[$i][$j]) < 32)
										exit(4);
										else
										{
											if ($regexpArray[$i][$j] == '[' || $regexpArray[$i][$j] == ']' || $regexpArray[$i][$j] == '^' || $regexpArray[$i][$j] == '$' || $regexpArray[$i][$j] == '\\' || $regexpArray[$i][$j] == '{' || $regexpArray[$i][$j] == '}' || $regexpArray[$i][$j] == '?' || $regexpArray[$i][$j] == '/')
												array_push($phpRegex,'\\');
												
											array_push($phpRegex, $regexpArray[$i][$j]);
											$state = "general";
										}
				break;
										
/*OperatorOR*/	case "OperatorOR":
					if ($regexpArray[$i][$j] == '.' || $regexpArray[$i][$j] == '*' || $regexpArray[$i][$j] == '+' || $regexpArray[$i][$j] == '|' || $regexpArray[$i][$j] == ')')
						$state = "error";
						else if (ord($regexpArray[$i][$j]) < 32)
							exit(4);
							else if ($regexpArray[$i][$j] == '(')
							{
								array_push($phpRegex, $regexpArray[$i][$j]);
								array_push($stack, $regexpArray[$i][$j]);
								$state = "general";
							} else if ($regexpArray[$i][$j] == ')')
								{
									array_push($stack, $regexpArray[$i][$j]);
									$state = "general";
									if (end($stack) == ')' && prev($stack) == '(') {
										array_pop($stack);
										array_pop($stack);
									}	
								} else  if ($regexpArray[$i][$j] == '!')
									{
										$state = "Negation";
									} else if ($regexpArray[$i][$j] == '%')
										{
											$state = "specialExpression";
										} else
										{
											if ($regexpArray[$i][$j] == '[' || $regexpArray[$i][$j] == ']' || $regexpArray[$i][$j] == '^' || $regexpArray[$i][$j] == '$' || $regexpArray[$i][$j] == '\\' || $regexpArray[$i][$j] == '{' || $regexpArray[$i][$j] == '}' || $regexpArray[$i][$j] == '?' || $regexpArray[$i][$j] == '/' || $regexpArray[$i][$j] == '\\')
												array_push($phpRegex,'\\');
									
											array_push($phpRegex, $regexpArray[$i][$j]);
													
											$state = "general";
										}
					break;
								
/*OperatorITERATE*/case "OperatorITERATE":
					if ($regexpArray[$i][$j] == '*' || $regexpArray[$i][$j] == '+')
						$state = "error";
						else if (ord($regexpArray[$i][$j]) < 32)
							exit(4);
								else if ($regexpArray[$i][$j] == '(')
								{
									array_push($phpRegex, $regexpArray[$i][$j]);
									array_push($stack, $regexpArray[$i][$j]);
									$state = "general";
								} else if ($regexpArray[$i][$j] == ')')
									{
										array_push($phpRegex, $regexpArray[$i][$j]);
										array_push($stack, $regexpArray[$i][$j]);
										$state = "general";
										if (end($stack) == ')' && prev($stack) == '(') {
											array_pop($stack);
											array_pop($stack);
										}	
									}					
									else  if ($regexpArray[$i][$j] == '!')
									{
										$state = "Negation";
									} else if ($regexpArray[$i][$j] == '%')
										{
											$state = "specialExpression";
										} else if ($regexpArray[$i][$j] == '.')
											{
												$state = "start";
											} else
												{
													if ($regexpArray[$i][$j] == '[' || $regexpArray[$i][$j] == ']' || $regexpArray[$i][$j] == '^' || $regexpArray[$i][$j] == '$' || $regexpArray[$i][$j] == '\\' || $regexpArray[$i][$j] == '{' || $regexpArray[$i][$j] == '}' || $regexpArray[$i][$j] == '?' || $regexpArray[$i][$j] == '/' || $regexpArray[$i][$j] == '\\')
															array_push($phpRegex,'\\');
													
													array_push($phpRegex, $regexpArray[$i][$j]);												
													$state = "general";
												}					
					break;		
					
/*SPECIAL*/		case "specialExpression":
					$state = "general";
					if ($regexpArray[$i][$j] == '*' || $regexpArray[$i][$j] == '+' || $regexpArray[$i][$j] == '|' || $regexpArray[$i][$j] == '.' || $regexpArray[$i][$j] == '!' || $regexpArray[$i][$j] == '%' || $regexpArray[$i][$j] == '(' || $regexpArray[$i][$j] == ')')
					{
						array_push($phpRegex, '\\');
						array_push($phpRegex, $regexpArray[$i][$j]);
					} else if ($regexpArray[$i][$j] == 's')
						{
							array_push($phpRegex, '\\');
							array_push($phpRegex, 's');
						} else if ($regexpArray[$i][$j] == 'a')
							{
								array_push($phpRegex, '.');
							} else if ($regexpArray[$i][$j] == 'd')
								{
									array_push($phpRegex, '\\');
									array_push($phpRegex, 'd');
								} else if ($regexpArray[$i][$j] == 'l')
									{
										array_push($phpRegex, '[');
										array_push($phpRegex, 'a');
										array_push($phpRegex, '-');
										array_push($phpRegex, 'z');
										array_push($phpRegex, ']');
									}  else if ($regexpArray[$i][$j] == 'L')
											{
												array_push($phpRegex, '[');
												array_push($phpRegex, 'A');
												array_push($phpRegex, '-');
												array_push($phpRegex, 'Z');
												array_push($phpRegex, ']');
											}  else if ($regexpArray[$i][$j] == 'w')
												{
													array_push($phpRegex, '[');
													array_push($phpRegex, 'a');
													array_push($phpRegex, '-');
													array_push($phpRegex, 'z');
													array_push($phpRegex, 'A');
													array_push($phpRegex, '-');
													array_push($phpRegex, 'Z');
													array_push($phpRegex, ']');
												}  else if ($regexpArray[$i][$j] == 'W')
														{
															array_push($phpRegex, '[');
															array_push($phpRegex, 'a');
															array_push($phpRegex, '-');
															array_push($phpRegex, 'z');
															array_push($phpRegex, 'A');
															array_push($phpRegex, '-');
															array_push($phpRegex, 'Z');
															array_push($phpRegex, ']');
															array_push($phpRegex, '|');
															array_push($phpRegex, '\\');
															array_push($phpRegex, 'd');
														}  else if ($regexpArray[$i][$j] == 't')
															{
																array_push($phpRegex, '\\');
																array_push($phpRegex, 't');
															} else if ($regexpArray[$i][$j] == 'n')
																{
																	array_push($phpRegex, '\\');
																	array_push($phpRegex, 'n');
																} else $state = "error";
									break;
									
/*NEGATION*/					case "Negation":
									
									if ($regexpArray[$i][$j] == '%')
									{
										$state = "NegationSpecialSymbol";
									} else if ($regexpArray[$i][$j] == '*' || $regexpArray[$i][$j] == '+' || $regexpArray[$i][$j] == '|' || $regexpArray[$i][$j] == '.' || $regexpArray[$i][$j] == '!' || $regexpArray[$i][$j] == ')')
										$state = "error";
										else if (ord($regexpArray[$i][$j]) < 32)
											exit(4);
											else 
											{
												array_push($phpRegex, '[');
												array_push($phpRegex, '^');
												
												if ($regexpArray[$i][$j] == '[' || $regexpArray[$i][$j] == ']' || $regexpArray[$i][$j] == '^' || $regexpArray[$i][$j] == '$' || $regexpArray[$i][$j] == '\\' || $regexpArray[$i][$j] == '{' || $regexpArray[$i][$j] == '}' || $regexpArray[$i][$j] == '?' || $regexpArray[$i][$j] == '.' || $regexpArray[$i][$j] == '/' || $regexpArray[$i][$j] == '\\')
														array_push($phpRegex,'\\');
												
												array_push($phpRegex, $regexpArray[$i][$j]);
												array_push($phpRegex, ']');
												$state = "general";
											}	
									break;
									
/*NEGATIONSPECIAL*/				case "NegationSpecialSymbol":
									
									$state = "general";
									array_push($phpRegex, '[');
									array_push($phpRegex, '^');
									
									if ($regexpArray[$i][$j] == '*' || $regexpArray[$i][$j] == '+' || $regexpArray[$i][$j] == '|' || $regexpArray[$i][$j] == '.' || $regexpArray[$i][$j] == '!' || $regexpArray[$i][$j] == '%' || $regexpArray[$i][$j] == '(' || $regexpArray[$i][$j] == ')')
									{
										array_push($phpRegex, '\\');
										array_push($phpRegex, $regexpArray[$i][$j]);
									} else if ($regexpArray[$i][$j] == 's')
										{
											array_push($phpRegex, '\\');
											array_push($phpRegex, 's');
										} else if ($regexpArray[$i][$j] == 'a')
											{
												array_push($phpRegex, '.');
											} else if ($regexpArray[$i][$j] == 'd')
												{
													array_push($phpRegex, '\\');
													array_push($phpRegex, 'd');
												} else if ($regexpArray[$i][$j] == 'l')
													{
														array_push($phpRegex, '(');
														array_push($phpRegex, 'a');
														array_push($phpRegex, '-');
														array_push($phpRegex, 'z');
														array_push($phpRegex, ')');
													}  else if ($regexpArray[$i][$j] == 'L')
														{
															array_push($phpRegex, '(');
															array_push($phpRegex, 'A');
															array_push($phpRegex, '-');
															array_push($phpRegex, 'Z');
															array_push($phpRegex, ')');
														}  else if ($regexpArray[$i][$j] == 'w')
															{
																array_push($phpRegex, '(');
																array_push($phpRegex, 'a');
																array_push($phpRegex, '-');
																array_push($phpRegex, 'z');
																array_push($phpRegex, 'A');
																array_push($phpRegex, '-');
																array_push($phpRegex, 'Z');
																array_push($phpRegex, ')');
															}  else if ($regexpArray[$i][$j] == 'W')
																{
																	array_push($phpRegex, '(');
																	array_push($phpRegex, 'a');
																	array_push($phpRegex, '-');
																	array_push($phpRegex, 'z');
																	array_push($phpRegex, 'A');
																	array_push($phpRegex, '-');
																	array_push($phpRegex, 'Z');
																	array_push($phpRegex, ')');
																	array_push($phpRegex, '|');
																	array_push($phpRegex, '\\');
																	array_push($phpRegex, 'd');
																}  else if ($regexpArray[$i][$j] == 't')
																	{
																		array_push($phpRegex, '\\');
																		array_push($phpRegex, 't');
																	} else if ($regexpArray[$i][$j] == 'n')
																		{
																			array_push($phpRegex, '\\');
																			array_push($phpRegex, 'n');
																		} else $state = "error";
									
									
									array_push($phpRegex, ']');
									break;
		}
		$j++;
	}
	$string1 = implode ($phpRegex);
	array_push($phpRegexArray,$string1);
	unset($phpRegex);
	$phpRegex = array();
	
	
	// Ak nie je spravne uzatvorkovany vyraz
	if (end($stack) == '$')
	{
		unset($stack);
		$stack = array('$');
	}	
		else
		{
			var_dump($stack);
			exit(4);
		}
			
	$i++;
	$j = 0;
}	

// Ak skonci v nekoncovom stave
if ($state == "start" || $state == "OperatorOR")
	exit(4);
				
$newstring = $inputFile;
$matches = array();
$indexArray = array();
$htmlEnd = array();

// Inicializacia pomocneho pola
for($l=0; $l < strlen($inputFile); $l++)
{
	$indexArray[$l] = $l;
}

$phpRegexArray = array_filter($phpRegexArray);
$phpRegexArray = array_slice($phpRegexArray, 0);

// Prechadza po regularnych vyrazoch
for ($i=0; $i<count($phpRegexArray); $i++) 
{
	preg_match_all('/'.$phpRegexArray[$i].'/su', $inputFile, $matches, PREG_OFFSET_CAPTURE );
		
	if (!$formatexpArrayOfArrays[$i])
		continue;
			
	// Prechadza po formatovacom poli	
	for ($j=0; $j< count($formatexpArrayOfArrays[$i]); $j++)	
	{
		// Prechadza po nalezoch
		for ($k=0; $k < count($matches[0]); $k++)
		{
			$match = $matches[0][$k][0];
			$position = $matches[0][$k][1];
			$actFormat = $formatexpArrayOfArrays[$i][$j];
												
			// Zistenie HTML tagov			
			if (preg_match('/^italic$/',$formatexpArrayOfArrays[$i][$j]))
			{
				$html1 = '<i>';
				$html2 = '</i>';
			}
			
			if (preg_match('/^teletype$/',$formatexpArrayOfArrays[$i][$j]))
			{
				$html1 = '<tt>';
				$html2 = '</tt>';
			}
			
			if (preg_match('/^bold$/',$formatexpArrayOfArrays[$i][$j]))
			{
				$html1 = '<b>';
				$html2 = '</b>';
			}
			
			if (preg_match('/^underline$/',$formatexpArrayOfArrays[$i][$j]))
			{
				$html1 = '<u>';
				$html2 = '</u>';
			}
						
			if (preg_match('/^size:[1-7]$/',$formatexpArrayOfArrays[$i][$j]))
			{
				$size = substr($formatexpArrayOfArrays[$i][$j], -1);
				$html1 = '<font size='.$size.'>';
				$html2 = '</font>';
			}
			
			if (preg_match('/^color:([a-zA-Z]|[0-9]){6}$/',$formatexpArrayOfArrays[$i][$j]))
			{
				$color = substr($formatexpArrayOfArrays[$i][$j], -6);
				$html1 = '<font color=#'.$color.'>';
				$html2 = '</font>';
			}
			
			if ($match != "") {
			
				// Vlozenie prveho tagu									
				$newstring = substr_replace($newstring, $html1, $indexArray[$position],0);
			
				// Uprava indexov pomocneho pola						
				for($m=$position; $m < strlen($inputFile); $m++)
				{
					$indexArray[$m] = $indexArray[$m] + strlen($html1);
				}
				
				// Vlozenie druheho tagu
				if ($position + strlen($match) < count($indexArray))
				{				
					if (($indexArray[$position+strlen($match)] - $indexArray[$position+strlen($match)-1]) > 1)
					{
						$newstring = substr_replace($newstring, $html2, $indexArray[$position+strlen($match)-1]+1,0);
					}
						else
						{
							$newstring = substr_replace($newstring, $html2, $indexArray[$position+strlen($match)],0);
						}
				} else
					{
							array_push($htmlEnd,$html2);
					}
								
				// Uprava indexov pomocneho pola			
				for ($n = $position + strlen($match); $n < strlen($inputFile); $n++)
				{
					$indexArray[$n] += strlen($html2);			
				}
			}
		}		
		
	}
}

$length = count($htmlEnd);

//Vlozenie vycnievajucich tagov
if ($length)
	for ($i = $length-1; $i >= 0; $i--)
	{
		$newstring = substr_replace($newstring, $htmlEnd[$i], strlen($newstring),0);
	}

// Argument --br
if ($br)
	$newstring = preg_replace('/\n/','<br />'.PHP_EOL,$newstring);

if ($output)
	file_put_contents($options['output'], $newstring);	
	else
		file_put_contents('php://stdout', $newstring);	

exit(0);
?>
