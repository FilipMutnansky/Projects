// fold2.cc
// Riesenie IJC-DU2, priklad a), 13.4.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: g++4.8
// Popis: Program zo zadaneho suboru cita
//		  text a formatuje ho parametrom -w 
//        na zadanu dlzku. 
//        Ak je zadany parameter -c zmeni 
//        maximalnu dlzku slova (80) na cislo
//		  zadane pri -w.

#include <iostream> 
#include <sstream>      // std::istringstream
#include <fstream>
#include <cstdlib> 

#define LINE_WIDTH 80

int getParams(int argc, char **argv, int *width, bool *cut, bool *file);

int main(int argc, char **argv)
{
	std::ios::sync_with_stdio(false);
	using namespace std;
	unsigned int lineLength = 0;
	unsigned int wordsOnLine = 0;
	unsigned int cutCount = 0;
	int width = 0;
	bool newLine = true;
	bool cut = false;
	bool file = false;
	string line;
	string word;
	filebuf textFile;	
		
	if (argc - 1 != 0)	
	{
		if (((string)argv[1] != "-w") && ((string)argv[1] != "-c"))		// ak nie su zadane parametre -w -c nacitava zo cin
		{				 
			textFile.open (argv[1],ios::in);
			
			if (!textFile.is_open()) 									// ak nastala chyba pri otvoreni
			{
				cerr << "CHYBA: Chyba pri otvoreni suboru" << endl;
				return 1;
			} else 
				{
					istream is(&textFile);
					cin.rdbuf(is.rdbuf());
				}
			
			file = true;
		}		
	}
	
	if (!width)
		width = LINE_WIDTH;
	
	if (getParams(argc, argv, &width, &cut, &file) == 1)	
	{
		cerr << "CHYBA: Chybne zadane argumenty.\n";
		return 1;
	}
			
	while (getline (cin,line))			// nacitavanie po riadkoch (zachovanie prazdnych riadkov)
	{
		istringstream iss(line);
							
		if (line.empty())						// ak je riadok prazdny -> odriadkuje
		{
			cout << endl;	
			newLine = true;
		}
		
		while(iss >> word) 						// nacitavanie po slove
		{
			wordsOnLine++;
			lineLength = lineLength + word.length();		// zvysenie dlzky riadku o slovo
			
			if (newLine == false)							// zaratanie medzery ako znaku
				lineLength++;
		
			if (lineLength > (unsigned int)width)					// prekrocenie limitu -> odriadkovanie
			{
				if (wordsOnLine != 1)						// v pripade ze je na riadku viac slov
					cout << endl;
				
				newLine = true;
				wordsOnLine=1;
				lineLength = word.length();					// "prenesenie" poctu znakov na dalsi riadok
			}
				
			if (newLine == false)
				cout << " ";										// odmedzerovanie
			
			if (cut == true)
			{			
				if ((word.length() > (unsigned int)width))       	    // oseknutie slova
				{
					word.erase(width);
					cutCount++;											// zamedzenie viacnasobnemu vypisu na cerr
				}
			}
						
			cout << word;											// vypis slova
			newLine = false;		
		}	
	}
	cout << endl;
	
	if (cut == true)
	{
		if (cutCount)
			cerr << "WARNING: Doslo k oseknutiu slova.\n";			// vypis na cerr
	}
	
	
	if (file) textFile.close();
}

int getParams(int argc, char **argv, int *width, bool *cut, bool *file)
{
	using namespace std;
	int i;
	unsigned int widthCount = 0, cutCount = 0;
	char *chyba;
	
	if (*file == true)
		i = 2;
			else
				i = 1;
	
	while (i <= argc-1)	
	{
		if ((string)argv[i] == "-w")
		{
			if ((i + 1) > (argc-1))
				return 1;
			
			i++;
			
			*width = strtol(argv[i],&chyba,10);	
			
			if ((*chyba != 0) || (*width <= 0))			// ak v cisle je nevhodny znak
				return 1;
				
			widthCount++;
			
		} else if ((string)argv[i] == "-c")  // CUT
			{		
				*cut = true;
				cutCount++;
			} else 
				return 1;				
					
		i++;		
	}	
	
	if (widthCount > 1 || cutCount > 1)			// viacnasobne zadane parametre
	{
			return 1;
	}
	
	return 0;
}
