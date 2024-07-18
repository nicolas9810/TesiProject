
#include "previsione.h"
#include "GreenPlantModel.h"
#include <iostream>
#include <cstdlib> 
#include <thread>
#include <chrono>
#include "tempo.h"


int main(int argc, char* argv[]) {

   

    

    GreenPlantModel model("./2018.csv");
    
    double time=0;
    int energiaMassimaAnnuale=0;
    int giornoEnergiaMassimaAnnuale=0;

    int energiaMinimaAnnuale=999999;
    int giornoEnergiaMinimaAnnuale=0;
    
    //potenza
    int potenzaMassimaAnnuale=90;
    int giornoPotenzaMassimaAnnuale=0;
    int orariopotenzaMassimaAnnuale=0;

    int potenzaMinimaAnnuale=9999;
    int giornoPotenzaMinimaAnnuale=0;
    int orariopotenzaMinimaAnnuale=0;
    int numero=atoi(argv[1]);
   

    printf("Hai inserito il numero: %d\n", numero);
    int cont=0;
    int base=0;
    if(numero == 0){
        cont = 360;
        base = 180;
    }
    else{cont = 180;
    base =0;

    }
   for(int i=base;i<cont;i++){
            int energiaCumulata=0;
            int maxEnergiaGiorno=0;
            int orarioMax=0;
            int minEnergiaGiorno=0;
            int orarioMin=0;
            if(i!=0)time+=24*60;
            std::cout << i << " [ ";
            for(int j=0;j<24;j++){
                int energia= model.getProducedPowerByTime(time);
                if(maxEnergiaGiorno<energia){
                    maxEnergiaGiorno=energia;
                    orarioMax=j;
                }
                
                energiaCumulata+=energia;
                std::cout << ", "<< energia ;
                time+=60;
            }
            std::cout << "]  energia max "<< maxEnergiaGiorno<< " alle ore "<< orarioMax<<"\n";
            if(potenzaMassimaAnnuale<maxEnergiaGiorno){
                giornoPotenzaMassimaAnnuale=i;
                potenzaMassimaAnnuale=maxEnergiaGiorno;
                orariopotenzaMassimaAnnuale=orarioMax;
            }
            if(potenzaMinimaAnnuale>maxEnergiaGiorno){
                giornoPotenzaMinimaAnnuale=i;
                potenzaMinimaAnnuale=maxEnergiaGiorno;
                orariopotenzaMinimaAnnuale=orarioMax;
            }
            if(energiaCumulata>=energiaMassimaAnnuale){
                energiaMassimaAnnuale=energiaCumulata;
                giornoEnergiaMassimaAnnuale=i;
            }
            if(energiaCumulata<=energiaMinimaAnnuale){
                energiaMinimaAnnuale=energiaCumulata;
                giornoEnergiaMinimaAnnuale=i;
            
            }
            

        
    }
   
    std::cout << " picco massimo energia  dell'anno "<<  potenzaMassimaAnnuale<<" nel giorno "<< giornoEnergiaMassimaAnnuale <<" alle ore "<< orariopotenzaMassimaAnnuale <<" \n";
    std::cout << " picco massimo energia  dell'anno "<<  potenzaMinimaAnnuale<<" nel giorno "<< giornoEnergiaMinimaAnnuale <<" alle ore "<< orariopotenzaMinimaAnnuale<<" \n";
    std::cout << " energia giornaleira massima dell'anno "<<  energiaMassimaAnnuale<<" nel giorno "<< giornoEnergiaMassimaAnnuale<<" \n";
    std::cout << " energia giornaliera minima dell'anno "<<  energiaMinimaAnnuale<<" nel giorno "<< giornoEnergiaMinimaAnnuale<<" \n";
    
    return 0;
}

    
