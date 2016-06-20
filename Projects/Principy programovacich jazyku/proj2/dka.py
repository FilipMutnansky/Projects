

#DKA:xmutna00
import argparse
import sys
#--------------------
parser = argparse.ArgumentParser(description='Process arguments')
parser.add_argument('-e','--no-epsilon-rules',action='count',default=False,dest='e',help='Odstranenie epsilon pravidiel')
parser.add_argument('-d','--determinization',action='count',default=False,dest='d',help='Determinizacia')
parser.add_argument('-i','--case-insensitive',action='count',default=False,dest='insensitive',help='Case insensitive')
parser.add_argument('--wsfa',action='count',default=False,dest='wsfa',help='Expansion')
parser.add_argument('--input',action='append',dest='input',help='Input file name')
parser.add_argument('--output',action='append',dest='output',help='Output file name')
parser.add_argument('--analyze-string',action='append',dest='stringToAnalyze',help='String to analyze')

try:
	results = parser.parse_args()
except SystemExit:
    if sys.argv[1] == "--help" and len(sys.argv) == 2:
        exit(0)
    else:
	    exit(1)

#Kontrola spravnosti argumentov
if (results.e > 1 or results.d > 1 or results.insensitive > 1 or results.wsfa > 1):
	exit(1)
	
if results.input != None and len(results.input) > 1:
	exit(1)
		
if results.output != None and len(results.output) > 1:
	exit(1)

if results.stringToAnalyze != None and len(results.stringToAnalyze) > 1:
	exit(1)

if results.e and results.d:
	exit(1)

if results.e and results.stringToAnalyze:
    exit(1)

if results.d and results.stringToAnalyze:
    exit(1)

if results.e and results.wsfa:
    exit(1)

if results.d and results.wsfa:
    exit(1)

#Otvorenie vstupneho suboru
try:
	if results.input != None:
		inputFile = open(results.input[0], 'r')
	else:
		inputFile = sys.stdin	
except IOError:
	exit(2)

#Otvorenie vystupneho suboru
try:
	if results.output != None:
		outputFile = open(results.output[0], 'w')
	else:
		outputFile = sys.stdout	
except IOError:
	exit(3)	

stateString = []
arrayOfStates = []
alphabetArray = []

startStateString = []
ruleStartStateArray = []

ruleEnterSymbolArray = []

endStateString = []
ruleEndStateArray = []

startStateString2 = []

endStateString2 = []
endStateArray = []
prevstate = ""

# Funkcia na preckocenie komentarov
def skipComment():
    while 1:
        c = inputFile.read(1)
        if not c or c == '\n': break
    return

#Zatvorenie suborov
def closeProgram(errnum):
	if inputFile is not sys.stdin:
	    inputFile.close()
    
	if outputFile is not sys.stdout:
	    outputFile.close()
	    
	exit(errnum)

# Funkcie na pridanie stringov do zoznamov
def appendToStateArray(stateString, arrayOfStates):
    stateString = ''.join(stateString)
    arrayOfStates.append(stateString)
    return

def appendToStartStateArray(startStateString, ruleStartStateArray):
    startStateString = ''.join(startStateString)
    ruleStartStateArray.append(startStateString)
    return
    
def appendToEndStateArray(endStateString, ruleEndStateArray):
    endStateString = ''.join(endStateString)
    ruleEndStateArray.append(endStateString)
    return
    
def appendToEndStateArray2(endStateString2, endStateArray):
    endStateString2 = ''.join(endStateString2)
    endStateArray.append(endStateString2)
    return

# Funkcia pre zoradenie stavov a spojenie znakom '_'
def createName(setOfNames):
    array = []
    array = list(setOfNames)
    array.sort()
    name = ""
    
    for i in range(len(array)):
        if i == 0:
            name = array[i]    
        else:
            name = name + "_" + array[i]

    return name
  

# KONECNY AUTOMAT PRE SPRACOVANIE VSTUPU  
state = "start"
while 1:
    char = inputFile.read(1)
    if not char and state != "THEEND":
        closeProgram(40)
        
    if not char: break
    
    if state == "start":		#START	
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "start"
        elif char == '(':
            state = "bracketOpen"
        else:
            closeProgram(40)           
            
    elif state == "bracketOpen": #BRACKET OPEN
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "bracketOpen"
        elif char == '{':
            state = "stateDefinition"
        else:
            closeProgram(40)
            
    elif state == "stateDefinition": #STATES DEFINITION
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "stateDefinition"
        elif char.isalpha():
            state = "stateName" 
            # append char to string
            if results.insensitive:
                stateString.append(char.lower()) 
            else:    
                stateString.append(char) 

        elif char == '}':
            if prevstate == "expSeparator":
                closeProgram(40)
            state = "stateEnd"
        else:
            closeProgram(40)
    
    elif state == "stateName":        #STATE NAME
        prevstate = state
        if char == '#':
            skipComment()
            # append string to array
            appendToStateArray(stateString, arrayOfStates)
            stateString = []
            state = "expSeparator"
        elif char.isspace():
        	# append string to array
            appendToStateArray(stateString, arrayOfStates)
            stateString = []
            state = "expSeparator"
        elif char.isalpha() or char.isdigit():
        	# append char to string
            if results.insensitive:
                stateString.append(char.lower())
            else:
                stateString.append(char)
        	
            state = "stateName"
        elif char == '}':
        	# append string to array
        	appendToStateArray(stateString, arrayOfStates)
        	stateString = []
        	state = "stateEnd"
        elif char == ',':
            # append string to array
            appendToStateArray(stateString, arrayOfStates)
            stateString = []
            prevstate = "expSeparator"
            state = "stateDefinition"
        else:
            closeProgram(40)
        	
    elif state == "expSeparator":       #EXPSEPARATOR
        prevstate = state
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "expSeparator"
        elif char == ',':
            state = "stateDefinition"
        elif char == '}':
        	state = "stateEnd"
        else:
            closeProgram(40)
    
    elif state == "stateEnd":           #STATEEND
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "stateEnd"
        elif char == ',':
            state = "beforeAlphabet"
        else:
            closeProgram(40)

    elif state == "beforeAlphabet":     #BEFOREALPHABET
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "beforeAlphabet"
        elif char == '{':
            state = "alphabetStart"
        else:
            closeProgram(40)

    elif state == "alphabetStart":      #ALPHABET START
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "alphabetStart"
        elif ord(char) == 39:
            state = "loadAlphabet"
        elif char == '}':
            if prevstate == "expSeparatorAlpha":
                closeProgram(40)
                
            state = "alphabetEnd"
        else:
            closeProgram(40)
    
    elif state == "loadAlphabet":       #LOADALPHABET
        if ord(char) == 39:
            state = "apostroph"
        else:
            state = "charLoaded"
            if results.insensitive:
                alphabetArray.append(char.lower())
            else:    
                alphabetArray.append(char)
            
    
    elif state == "charLoaded":         #CHARLOADED
        if ord(char) == 39:
            state = "expSeparatorAlpha"
        else:
            closeProgram(40)
    
    elif state == "apostroph":          #APOSTROPH
        if ord(char) == 39:
            alphabetArray.append(chr(39))
            state = "charLoaded"
        else:
            closeProgram(40)
    
    elif state == "expSeparatorAlpha":  #EXPSEPARATORALPHA
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "expSeparatorAlpha"
        elif char == ',':
            prevstate = state
            state = "alphabetStart"
        elif char == '}':
            state = "alphabetEnd"
        else:
            closeProgram(40)
    
    elif state == "alphabetEnd":        #ALPHABETEND
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "alphabetEnd"
        elif char == ',':
            state = "beforeRules"
        else:
            closeProgram(40)    
          
    elif state == "beforeRules":        #BEFORERULES
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "beforeRules"
        elif char == '{':
            state = "rulesStart"
        else:
            closeProgram(40)
            
    elif state == "rulesStart":         #RULESSTART
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "rulesStart"
        elif char.isalpha():
            state = "ruleStartStateName" 
            # append char to string
            if results.insensitive:
                startStateString.append(char.lower()) 
            else:    
                startStateString.append(char)             
        elif char == '}':
            state = "rulesEnd"
            if prevstate == "expRuleSeparator":
                closeProgram(40)
        else:
            closeProgram(40)
     
    elif state == "ruleStartStateName":     #RULESTARTSTATENAME
        if char == '#':
            skipComment()
            # append string to array
            appendToStartStateArray(startStateString, ruleStartStateArray)
            startStateString = []
            state = "expEntrySymbol"
        elif char.isspace():
        	# append string to array
            appendToStartStateArray(startStateString, ruleStartStateArray)
            startStateString = []
            state = "expEntrySymbol"
        elif char.isalpha() or char.isdigit():
        	# append char to string
            if results.insensitive:
                startStateString.append(char.lower())
            else:
                startStateString.append(char)
            state = "ruleStartStateName"
        elif ord(char) == 39:
            # append string to array
            appendToStartStateArray(startStateString, ruleStartStateArray)
            startStateString = []
            state = "ruleLoadSymbol"
        else:
            closeProgram(40)
    
    elif state == "expEntrySymbol":     #EXPENTRYSYMBOL
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "expEntrySymbol"
        elif ord(char) == 39: 
            state = "ruleLoadSymbol"           
        else:
            closeProgram(40)
            
    elif state == "ruleLoadSymbol":        #RULELOADSYMBOL
        if ord(char) == 39:
            state = "ruleApostroph"
        else:
            state = "SymbolLoaded"
            if results.insensitive:
                ruleEnterSymbolArray.append(char.lower())
            else:    
                ruleEnterSymbolArray.append(char) 
    elif state == "ruleApostroph":              #RULEAPOSTROPH
        if ord(char) == 39:
            ruleEnterSymbolArray.append(chr(39))
            state = "SymbolLoaded"
        elif char == '-':
            ruleEnterSymbolArray.append('')
            state = "expArrow2"
        elif char.isspace():
            ruleEnterSymbolArray.append('')
            state = "expArrow1"
        else:
            closeProgram(40)
    
    elif state == "SymbolLoaded":               #SYMBOLLOADED
        if ord(char) == 39:
            state = "expArrow1"
        else:
            closeProgram(40)
    
    elif state == "expArrow1":                  #EXPARROW1
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "expArrow1"
        elif char == '-':
        	state = "expArrow2"
        else:         
            closeProgram(40)
    
    elif state == "expArrow2":                  #EXPARROW2
        if char == '>':
            state = "expEndState"
        else:
            closeProgram(40)
    
    elif state == "expEndState":                #EXPENDSTATE
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "expEndState"
        elif char.isalpha():
            state = "ruleEndStateName" 
            # append char to string
            if results.insensitive:
                endStateString.append(char.lower()) 
            else:    
                endStateString.append(char) 
            
        else:
            closeProgram(40)
    
    elif state == "ruleEndStateName":           #RULEENDSTATENAME
        if char == '#':
            skipComment()
            # append string to array
            appendToEndStateArray(endStateString, ruleEndStateArray)
            endStateString = []
            state = "expRuleSeparator"
        elif char.isspace():
        	# append string to array
            appendToEndStateArray(endStateString, ruleEndStateArray)
            endStateString = []
            state = "expRuleSeparator"
        elif char.isalpha() or char.isdigit():
        	# append char to string
            if results.insensitive:
                endStateString.append(char.lower()) 
            else:    
                endStateString.append(char)
            state = "ruleEndStateName"
        elif char == ',':
            # append string to array
            appendToEndStateArray(endStateString, ruleEndStateArray)
            endStateString = []
            prevstate = "expRuleSeparator"
            state = "rulesStart"
        elif char == '}':
            # append string to array
            appendToEndStateArray(endStateString, ruleEndStateArray)
            endStateString = []
            state = "rulesEnd"
        else:
            closeProgram(40)
            
    elif state == "expRuleSeparator":           #EXPRULESEPARATOR
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "expRuleSeparator"
        elif char == ',': 
            prevstate = state 
            state = "rulesStart"
        elif char == '}':
            state = "rulesEnd"             
        else:
            closeProgram(40)

    elif state == "rulesEnd":                   #RULESEND
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "rulesEnd"
        elif char == ',':
            state = "beforeStartState"
        else:
            closeProgram(40)   

    elif state == "beforeStartState":           #BEFORESTARTSTATE
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "beforeStartState"
        elif char.isalpha():
            state = "startStateName" 
            # append char to string
            if results.insensitive:
                startStateString2.append(char.lower())  
            else:    
                startStateString2.append(char)
        else:
            closeProgram(40)
     
    elif state == "startStateName":             #STARTSTATENAME
        if char == '#':
            skipComment()
            startStateString2 = ''.join(startStateString2)
            state = "expStartStateEnd"
        elif char.isspace():
            startStateString2 = ''.join(startStateString2)
            state = "expStartStateEnd"
        elif char.isalpha() or char.isdigit():
        	# append char to string
            if results.insensitive:
                startStateString2.append(char.lower())  
            else:    
                startStateString2.append(char)
            state = "startStateName"
        elif char == ',':
            startStateString2 = ''.join(startStateString2)
            state = "endStartState"
        else:
            closeProgram(40)

    elif state == "expStartStateEnd":           #EXPSTARTSTATEEND
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "expStartStateEnd"
        elif char == ',': 
            state = "endStartState"
        else:
            closeProgram(40)
            
    elif state == "endStartState":              #ENDSTARTSTATE
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "endStartState"
        elif char == '{': 
            state = "endStatesStart"
        else:
            closeProgram(40)

    elif state == "endStatesStart":             #ENDSTATESSTART
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "endStatesStart"
        elif char.isalpha():
            state = "endStateName" 
            # append char to string
            if results.insensitive:
                endStateString2.append(char.lower())
            else:    
                endStateString2.append(char)
            
        elif char == '}':
            if prevstate == "expSeparator2":
                closeProgram(40)
            state = "endStateEnd"
        else:
            closeProgram(40)   
            
    elif state == "endStateName":               #ENDSTATENAME
        if char == '#':
            skipComment()
            # append string to array
            appendToEndStateArray2(endStateString2, endStateArray)
            endStateString2 = []
            state = "expSeparator2"
        elif char.isspace():
        	# append string to array
            appendToEndStateArray2(endStateString2, endStateArray)
            endStateString2 = []
            state = "expSeparator2"
        elif char.isalpha() or char.isdigit():
        	# append char to string
            if results.insensitive:
                endStateString2.append(char.lower())
            else:    
                endStateString2.append(char)
            state = "endStateName"
        elif char == '}':
        	# append string to array
        	appendToEndStateArray2(endStateString2, endStateArray)
        	endStateString2 = []
        	state = "endStateEnd"
        elif char == ',':
            # append string to array
            appendToEndStateArray2(endStateString2, endStateArray)
            endStateString2 = []
            prevstate = "expSeparator2"
            state = "endStatesStart"
        else:
            closeProgram(40)        

    elif state == "expSeparator2":          #EXPSEPARATOR2
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "expSeparator2"
        elif char == ',':
            prevstate = state
            state = "endStatesStart"
        elif char == '}':
        	state = "endStateEnd"
        else:
            closeProgram(40)
            
    elif state == "endStateEnd":            #ENDSTATEEND
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "endStateEnd"
        elif char == ')':
        	state = "THEEND"
        else:
            closeProgram(40)
            
    elif state == "THEEND":                 #THEEND
        if char == '#':
            skipComment()
        elif char.isspace():
            state = "THEEND"
        else:
            closeProgram(40)
 
#Vytvorenie mnozin    
setOfStates = set(arrayOfStates)
setOfAlphabet = set(alphabetArray)
setOfEndStates = set(endStateArray)

#Ak je abeceda prazdna
if not alphabetArray:
    closeProgram(41)

#Ak mnozina koncovych stavov nie je podmnozinou stavov
if not setOfEndStates.issubset(setOfStates):
    closeProgram(41)

#Ak zaciatocny stav pravidla nie je v mnozine stavov    
for i in range(len(ruleStartStateArray)):
    if not ruleStartStateArray[i] in setOfStates:
       closeProgram(41)

#Ak vstupny symbol pravidla nie je v abecede
for i in range(len(ruleEnterSymbolArray)):
    if not set(ruleEnterSymbolArray[i]).issubset(setOfAlphabet):
       closeProgram(41)

#Ak koncovy stav pravidla nie je v mnozine stavov    
for i in range(len(ruleEndStateArray)):
    if not ruleEndStateArray[i] in setOfStates:
       closeProgram(41)
   
#Ak zaciatocny stav nie je v mnozine stavov    
if not startStateString2 in setOfStates:
    closeProgram(41)

#Vytvorenie zoznamu pravidiel
rules = [list(ruleStartStateArray) for ruleStartStateArray in zip(ruleStartStateArray, ruleEnterSymbolArray, ruleEndStateArray)]
rulesTuple = set(tuple(element) for element in rules)
rulesSet = set(rulesTuple)
rules = [list(rulesSet) for rulesSet in set(tuple(element) for element in rulesSet)]


uzavery = []
uzaver = []

#Urci epsilon uzavery pre kazdy stav
if results.e or results.d or results.stringToAnalyze or results.wsfa:
    for i in range(len(list(setOfStates))):
        uzaver = []
        uzaver.append(list(setOfStates)[i])
        for j in range(len(rules)):
            if rules[j][0] == list(setOfStates)[i] and rules[j][1] == '':
                uzaver.append(rules[j][2])
    
        uzavery.append(uzaver)
    
    for i in range(len(uzavery)):
        j=0
        while (j < len(uzavery[i])):
            for k in range(len(rules)):
                if rules[k][0] == uzavery[i][j] and rules[k][1] == '' and not rules[k][2] in uzavery[i]:
                    uzavery[i].append(rules[k][2])
            j = j+1        


    l = 0
    ruleStartStateArrayNew = []
    ruleEnterSymbolArrayNew = []
    ruleEndStateArrayNew = []

    #Urci nove pravidla bez epsilon prechodov
    for i in range(len(list(setOfStates))):
        for j in range(len(uzavery[i])):
            for k in range(len(rules)):
                if rules[k][0] == uzavery[i][j] and rules[k][1] != '':
                    ruleStartStateArrayNew.append(list(setOfStates)[i])
                    ruleEnterSymbolArrayNew.append(rules[k][1])
                    ruleEndStateArrayNew.append(rules[k][2])
                    l = l + 1


    rulesNew = [list(ruleStartStateArrayNew) for ruleStartStateArrayNew in zip(ruleStartStateArrayNew, ruleEnterSymbolArrayNew, ruleEndStateArrayNew)]
    rulesNewTuple = set(tuple(element) for element in rulesNew)
    rulesNewSet = set(rulesNewTuple)
    rulesNew = [list(rulesNewSet) for rulesNewSet in set(tuple(element) for element in rulesNewSet)]

    #urcit konecne stavy podla uzaverov
    endStatesNew = []

    for i in range(len(list(setOfStates))):
        if setOfEndStates.intersection(set(uzavery[i])):
            endStatesNew.append(list(setOfStates)[i])
    
    endStatesNew = set(endStatesNew)    

# VYPIS PRE ODSTRANENIE EPSILON PRECHODOV
if results.e:
    print('(',file=outputFile)
    print('{',end="",file=outputFile)
    statesList = list(setOfStates)
    statesList.sort()
    
    for i in range(len(statesList)):
        print(statesList[i],end="",file=outputFile)
        if i != (len(statesList)-1):
            print(', ',end="",file=outputFile)
        else:
            print('},',file=outputFile)
    
    alphabetArray = list(setOfAlphabet)
    alphabetArray.sort()

    print('{',end="",file=outputFile)
    for i in range(len(alphabetArray)):
        print(chr(39),end="",file=outputFile)

        if alphabetArray[i] != chr(39):
            print(alphabetArray[i],end="'",file=outputFile)
        else:
            print(alphabetArray[i],end="''",file=outputFile)

        if i != (len(alphabetArray)-1):
            print(', ',end="",file=outputFile)
        else:
            print('},',file=outputFile)
    
    print('{',file=outputFile)
    
    rulesNew.sort()

    for i in range(len(rulesNew)):
        print(rulesNew[i][0],end=" ",file=outputFile)
        print(chr(39),end="",file=outputFile)
        
        if rulesNew[i][1] != chr(39):
            print(rulesNew[i][1],end="' ",file=outputFile)
        else:
            print(rulesNew[i][1],end="'' ",file=outputFile)
            
        print('->',end=" ",file=outputFile)
        print(rulesNew[i][2],end="",file=outputFile)
        if i != (len(rulesNew)-1):
            print(',',file=outputFile)
        else:
            print('',file=outputFile)
            
    print('},',file=outputFile)
    
    print(startStateString2,end="",file=outputFile)
    print(',',file=outputFile)
    print('{',end="",file=outputFile)

    endStatesNew = list(endStatesNew)
    endStatesNew.sort()

    for i in range(len(endStatesNew)):
        print(endStatesNew[i],end="",file=outputFile)
        if i != (len(endStatesNew)-1):
            print(', ',end="",file=outputFile)
            
    print('}',file=outputFile)

    print(')',end="",file=outputFile)
    closeProgram(0)

# NORMALIZOVANY VYPIS AUTOMATU
if not results.e and not results.d and not results.stringToAnalyze and not results.wsfa:
    print('(',file=outputFile)
    print('{',end="",file=outputFile)
    statesList = list(setOfStates)
    statesList.sort()
    
    for i in range(len(statesList)):
        print(statesList[i],end="",file=outputFile)
        if i != (len(statesList)-1):
            print(', ',end="",file=outputFile)
        else:
            print('},',file=outputFile)
    
    alphabetArray = list(setOfAlphabet)
    alphabetArray.sort()

    print('{',end="",file=outputFile)
    
    for i in range(len(alphabetArray)):
        print(chr(39),end="",file=outputFile)

        if alphabetArray[i] != chr(39):
            print(alphabetArray[i],end="'",file=outputFile)
        else:
            print(alphabetArray[i],end="''",file=outputFile)

        if i != (len(alphabetArray)-1):
            print(', ',end="",file=outputFile)
        else:
            print('},',file=outputFile)
    
    print('{',file=outputFile)
    
    rules.sort()

    for i in range(len(rules)):
        print(rules[i][0],end=" ",file=outputFile)
        print(chr(39),end="",file=outputFile)
        
        if rules[i][1] != chr(39):
            print(rules[i][1],end="' ",file=outputFile)
        else:
            print(rules[i][1],end="'' ",file=outputFile)
        print('->',end=" ",file=outputFile)
        print(rules[i][2],end="",file=outputFile)
        if i != (len(rules)-1):
            print(',',file=outputFile)
        else:
            print('',file=outputFile)

    print('},',file=outputFile)    

    print(startStateString2,end="",file=outputFile)
    print(',',file=outputFile)
    print('{',end="",file=outputFile)


    endStates = list(setOfEndStates)
    endStates.sort()

    for i in range(len(endStates)):
        print(endStates[i],end="",file=outputFile)
        if i != (len(endStates)-1):
            print(', ',end="",file=outputFile)

    print('}',file=outputFile)

    print(')',end="",file=outputFile)
    closeProgram(0)

# DETERMINIZACIA
newStates = set()                   # Mnozina stavov pre novy automat
alphabetArray = list(setOfAlphabet) # Zoznam abecedy
stackOfStates = []                  # Zasobnik mnozin stavov, vypocet prebieha kym nie je prazdny
setOfStatesInState = set()          # Mnozina reprezentujuca zhluk stavov
setOfEndStatesNew = set()
setOfStatesInState.add(startStateString2)
stackOfStates.append(setOfStatesInState)

rulesStartState = []
rulesEntrySymbol = []
rulesEndState = []

k=0

while k < len(stackOfStates):
    for i in range(len(alphabetArray)):
        
        setOfStatesInState=set()
        
        for j in range(len(rulesNew)):
            if rulesNew[j][0] in stackOfStates[k] and rulesNew[j][1] == alphabetArray[i]:
                setOfStatesInState.add(rulesNew[j][2])
        
        newStates.add(createName(stackOfStates[k]))          # pridanie stavu do novej mnoziny stavov
        
        if endStatesNew.intersection(stackOfStates[k]):
            setOfEndStatesNew.add(createName(stackOfStates[k]))

        if setOfStatesInState:
            rulesStartState.append(createName(stackOfStates[k])) # pridanie stavu do novej mnoziny pravidiel
        
            if setOfStatesInState not in stackOfStates:
                stackOfStates.append(setOfStatesInState)

            rulesEntrySymbol.append(alphabetArray[i])
            rulesEndState.append(createName(setOfStatesInState))

    k = k+1


rulesNew = [list(rulesStartState) for rulesStartState in zip(rulesStartState, rulesEntrySymbol, rulesEndState)]
rulesNewTuple = set(tuple(element) for element in rulesNew)
rulesNewSet = set(rulesNewTuple)
rulesNew = [list(rulesNewSet) for rulesNewSet in set(tuple(element) for element in rulesNewSet)]
statesList = list(newStates)
statesList.sort()

# STR
if results.stringToAnalyze:

    if not results.stringToAnalyze[0]:
        if startStateString2 not in setOfEndStatesNew:
            print(0,end="",file=outputFile)
        else:
            print(1,end="",file=outputFile)
        
        closeProgram(0)

    listOfStates = []
    listOfStates.append(startStateString2)
    k=0
    while k < len(listOfStates):
        nextState = ""
        for i in range(len(rulesNew)):
            if results.stringToAnalyze[0][k] not in setOfAlphabet:
                closeProgram(1)
        
            if results.stringToAnalyze[0][k] == rulesNew[i][1] and rulesNew[i][0] == listOfStates[k]:
                nextState = rulesNew[i][2]
        
        if nextState == "":
            print(0,end="",file=outputFile)
            closeProgram(0)

        listOfStates.append(nextState)
        k = k+1
        if k == len(results.stringToAnalyze[0]):
            break

    if listOfStates[k] not in setOfEndStatesNew:
        print(0,end="",file=outputFile)
    else:
        print(1,end="",file=outputFile)

    closeProgram(0)

#WSFA
if results.wsfa:

    if results.insensitive:
        qFalse = "qfalse"
    else:
        qFalse = "qFALSE"

    k = 0
    wsfaList = []
    wsfaList = list(setOfEndStatesNew)
    rulesStartStateWsfa = []
    rulesEntrySymbolWsfa = []
    rulesEndStateWsfa = []
    newStates2 = set(setOfEndStatesNew)
    
    while k < len(wsfaList):
        for j in range(len(rulesNew)):
            if rulesNew[j][2] == wsfaList[k]:
                newStates2.add(rulesNew[j][0])
                rulesStartStateWsfa.append(rulesNew[j][0])
                rulesEntrySymbolWsfa.append(rulesNew[j][1])
                rulesEndStateWsfa.append(rulesNew[j][2])
            
                if rulesNew[j][0] not in wsfaList:
                    wsfaList.append(rulesNew[j][0])
    
        k = k+1
    
    rulesNewWsfa = [list(rulesStartStateWsfa) for rulesStartStateWsfa in zip(rulesStartStateWsfa, rulesEntrySymbolWsfa, rulesEndStateWsfa)]
    rulesNewWsfaTuple = set(tuple(element) for element in rulesNewWsfa)
    rulesNewWsfaSet = set(rulesNewWsfaTuple)
    rulesNewWsfa = [list(rulesNewWsfaSet) for rulesNewWsfaSet in set(tuple(element) for element in rulesNewWsfaSet)]

    newStates2 = list(newStates2)
    found = 0

    for i in range(len(newStates2)):
        for k in range (len(alphabetArray)):
            found = 0
            for j in range(len(rulesNewWsfa)):
                if rulesNewWsfa[j][0] == newStates2[i] and rulesNewWsfa[j][1] == alphabetArray[k]:
                    found = 1
        
            if found == 0:
                rulesStartStateWsfa.append(newStates2[i])
                rulesEntrySymbolWsfa.append(alphabetArray[k])
                rulesEndStateWsfa.append(qFalse)
    
    for i in range(len(alphabetArray)):
        rulesStartStateWsfa.append(qFalse)
        rulesEntrySymbolWsfa.append(alphabetArray[i])
        rulesEndStateWsfa.append(qFalse)

    rulesNewWsfa = [list(rulesStartStateWsfa) for rulesStartStateWsfa in zip(rulesStartStateWsfa, rulesEntrySymbolWsfa, rulesEndStateWsfa)]
    rulesNewWsfaTuple = set(tuple(element) for element in rulesNewWsfa)
    rulesNewWsfaSet = set(rulesNewWsfaTuple)
    rulesNewWsfa = [list(rulesNewWsfaSet) for rulesNewWsfaSet in set(tuple(element) for element in rulesNewWsfaSet)]
    
    if startStateString2 not in newStates2:
        startStateString2 = qFalse
    
    newStates2.append(qFalse)
    newStates2.sort()
    statesList = newStates2
    rulesNew = rulesNewWsfa


# PRINT -d, WSFA
print('(',file=outputFile)
print('{',end="",file=outputFile)
    
for i in range(len(statesList)):
    print(statesList[i],end="",file=outputFile)
    if i != (len(statesList)-1):
        print(', ',end="",file=outputFile)
    else:
        print('},',file=outputFile)
    
alphabetArray = list(setOfAlphabet)
alphabetArray.sort()

print('{',end="",file=outputFile)
for i in range(len(alphabetArray)):
    print(chr(39),end="",file=outputFile)

    if alphabetArray[i] != chr(39):
        print(alphabetArray[i],end="'",file=outputFile)
    else:
        print(alphabetArray[i],end="''",file=outputFile)

    if i != (len(alphabetArray)-1):
        print(', ',end="",file=outputFile)
    else:
        print('},',file=outputFile)
    
print('{',file=outputFile)
    
rulesNew.sort()

for i in range(len(rulesNew)):
    print(rulesNew[i][0],end=" ",file=outputFile)
    print(chr(39),end="",file=outputFile)
        
    if rulesNew[i][1] != chr(39):
        print(rulesNew[i][1],end="' ",file=outputFile)
    else:
        print(rulesNew[i][1],end="'' ",file=outputFile)
            
    print('->',end=" ",file=outputFile)
    print(rulesNew[i][2],end="",file=outputFile)
    if i != (len(rulesNew)-1):
        print(',',file=outputFile)
    else:
        print('',file=outputFile)
        
print('},',file=outputFile)
    
print(startStateString2,end="",file=outputFile)
print(',',file=outputFile)
print('{',end="",file=outputFile)

endStatesNew = list(setOfEndStatesNew)
endStatesNew.sort()

for i in range(len(endStatesNew)):
    print(endStatesNew[i],end="",file=outputFile)
    if i != (len(endStatesNew)-1):
        print(', ',end="",file=outputFile)
       
print('}',file=outputFile)

print(')',end="",file=outputFile)
closeProgram(0)






