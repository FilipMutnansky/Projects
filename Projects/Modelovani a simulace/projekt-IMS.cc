#include "simlib.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stdlib.h>

using namespace std;

const int VykrmKapacita = 220;
const int kapacitaLiahen = 1200;

Facility Liahen[4];
Store Vykrm[20];

double trzby = 0;
double naklady = 0;
double zisk = 0;

int pocetProcesov[20] = {};
int poradieProcesu[20] = {};
int isBusy[20] = {};
int celkovePoradie = 0;
int rezim, nakup = 0;

double cenaVyliahnuteho = 6.50;

ofstream file;

void printTrzby(int startTime)
{
    if (Time >= startTime)
        file << Time << " " << int(trzby) << endl;
}

class ZivotKurata : public Process { 
    void Behavior() {
        for (int i = 0; i < 20; i++)
        {
            if (Vykrm[i].Full() == 0 && isBusy[i] == 0)
            {
                poradieProcesu[i]++;
                
                // Obsadenie skladu
                if (poradieProcesu[i] == pocetProcesov[i])
                    isBusy[i] = 1;

                Enter(Vykrm[i],1);
                Wait(36);
                
                poradieProcesu[i] = 0;
                pocetProcesov[i] = 0;
                isBusy[i] = 0;

                // Krmivo (konverzia krmiva = 1.8)
                trzby -= 3.6*7.00;
                naklady += 3.6*7.00;                

                Leave(Vykrm[i],1);
                break;
            }
        }

        if (Uniform(0,100) > 3) // UHYN POCAS VYKRMU 3%
        {   
            // PORAZKA
            if (Uniform(0,100) < 30)  // CELE KURA
            {
                trzby += 1.47*46.77;
                zisk += 1.47*46.77;
            } else                    // PORCOVANE KURA
            {
                trzby += 0.318*104.21 + 53.63*0.33;
                zisk += 0.318*104.21 + 53.63*0.33;
            }
        } else  // KURA NEPREZILO VYKRM
            return;    

    }
};

class ZivotKurataKupeneho : public Process { 
    int cisloFarmy;
    void Behavior() {
        trzby -= cenaVyliahnuteho;
        naklady += cenaVyliahnuteho;

        if (rezim == 1)
            cisloFarmy = 2;
        else if (rezim == 3)
            cisloFarmy = 6;
        else if (rezim == 4)
            cisloFarmy = 8;

        for (int i = cisloFarmy; i < 20; i++)
        {
            if (Vykrm[i].Full() == 0)
            {
                Enter(Vykrm[i],1);
                Wait(36);                
                
                // Krmivo (konverzia krmiva = 1.8/kg)    
                trzby -= 3.6*7.00;
                naklady += 3.6*7.00;                

                Leave(Vykrm[i],1);
                break;
            }
        }

        if (Uniform(0,100) > 3) // UHYN POCAS VYKRMU 3%
        {   
            // PORAZKA
            if (Uniform(0,100) < 30)  // CELE KURA
            {
                trzby += 1.47*46.77;
                zisk += 1.47*46.77;
            } else                    // PORCOVANE KURA
            {
                trzby += 0.318*104.21 + 53.63*0.33;
                zisk += 0.318*104.21 + 53.63*0.33;
            }
        } else  // KURA NEPREZILO VYKRM
            return;    

    }
};

class Hatching : public Process { 
  int pocetVyliahnutych;
  int nasadeneVajcia;
  void Behavior() {    
    
    if (rezim == 3) 
        nasadeneVajcia = kapacitaLiahen/3;
    else if (rezim == 4)
        nasadeneVajcia = kapacitaLiahen/4;
    else if (rezim == 1)
        nasadeneVajcia = kapacitaLiahen;

    trzby -= nasadeneVajcia*1.96; // Nakup vajec
    naklady += nasadeneVajcia*1.96; // Nakup vajec

    for (int i = 0; i < rezim; i++)
    {
        if (!Liahen[i].Busy())
        {
            Seize(Liahen[i]);
            Wait(18);
            Release(Liahen[i]);
            
            Wait(3);
            
            pocetVyliahnutych = int((nasadeneVajcia/100.0)*Uniform(85,90));
            break;
        }
    }

    if (pocetVyliahnutych > VykrmKapacita)  // POCET NAD KAPACITU -> PREDAJ ZVYSKU
    {
        trzby += (pocetVyliahnutych - VykrmKapacita)*cenaVyliahnuteho;
        zisk += (pocetVyliahnutych - VykrmKapacita)*cenaVyliahnuteho;
        for (int i = 0; i < VykrmKapacita; i++)
            (new ZivotKurata)->Activate();
    } else
    {
        for (int i = 0; i < pocetVyliahnutych; i++)
            (new ZivotKurata)->Activate();
        
        pocetProcesov[celkovePoradie] = pocetVyliahnutych;
    }   

    celkovePoradie++;

  }
};

class Generator1 : public Event {  
  void Behavior() {
    (new Hatching)->Activate();   
    Activate(Time+18);
  }
};

class Generator3 : public Event {  
  void Behavior() {
    (new Hatching)->Activate();   
    Activate(Time+6);
  }
};

class Generator4 : public Event {  
  void Behavior() {
    (new Hatching)->Activate();   
    Activate(Time+4.5);
  }
};

class GeneratorKupenych : public Event {  
  int pocetFariem;
  void Behavior() {

    pocetFariem = 20 - 2*rezim;

    for (int i = 0; i < VykrmKapacita*pocetFariem; i++)           
      (new ZivotKurataKupeneho)->Activate()  ;   
    
    Activate(Time+36);
  }
};

class WriteTrzby : public Event {  
  void Behavior() {
    printTrzby(57);            
    Activate(Time+10);
  }
};

class Pay : public Event {  
  void Behavior() {
    if (Time >= 57) 
    {
        trzby -= 0.55*800;
        zisk -= 0.55*800;
    }
        else
        {
            trzby -= 0.55*65;
            zisk -= 0.55*65;
        }

    Activate(Time+1);
  }
};

int main(int argc, char** argv) {    
  RandomSeed(time(NULL));

  SetOutput("projekt.out");  

  if (argc -1 >= 3 && (argv[3][0] == 'a'))
    SetOutput("experiment1a");
  else if (argc -1 >= 3 && (argv[3][0] == 'b'))
    SetOutput("experiment1b");
  else if (argc -1 >= 3 && (argv[3][0] == 'c'))    
    SetOutput("experiment1c");
  else if (argc -1 >= 3 && (atoi(argv[3]) == 2)) file.open("experiment2", std::ios_base::app);
  else if (argc -1 >= 3 && (atoi(argv[3]) == 3)) file.open("experiment3", std::ios_base::app);
  else if (argc -1 >= 3 && (atoi(argv[3]) == 4)) file.open("experiment4", std::ios_base::app);
  
  for (int i = 0; i < 20; i++)
    Vykrm[i].SetCapacity(VykrmKapacita);
    
  rezim = atoi(argv[1]);

  if (argc - 1 >= 4)
    cenaVyliahnuteho = atof(argv[4]);

  Init(0,400);    
   
  (new WriteTrzby)->Activate(); 
  (new Pay)->Activate(); 

  if (argc - 1 >= 2)
    if (atoi(argv[2]) == 1)
     (new GeneratorKupenych)->Activate();  

  if (atoi(argv[1]) == 4)
    (new Generator4)->Activate();
  else if (atoi(argv[1]) == 3)
    (new Generator3)->Activate();
  else if (atoi(argv[1]) == 1)
    (new Generator1)->Activate();

  Run();             

  for (int i = 0; i < 20; i++)
    Vykrm[i].Output();
  
  file << endl;
  file.close();
  return 0;
}
