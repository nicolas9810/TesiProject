
#include "GreenPlantModel.h"
#include <iostream>
#include <cstdlib> 
#include <thread>
#include <chrono>


int main(int argc, char* argv[]) {

   

    

    GreenPlantModel model("../csv/2018.csv");
    
    double time=0;
    int mese =atoi(argv[1]);
   
     switch (mese) {
        case 0:
            printf("Gennaio\n");
            break;
        case 1:
            printf("Febbraio\n");
            break;
        case 2:
            printf("Marzo\n");
            break;
        case 3:
            printf("Aprile\n");
            break;
        case 4:
            printf("Maggio\n");
            break;
        case 5:
            printf("Giugno\n");
            break;
        case 6:
            printf("Luglio\n");
            break;
        case 7:
            printf("Agosto\n");
            break;
        case 8:
            printf("Settembre\n");
            break;
        case 9:
            printf("Ottobre\n");
            break;
        case 10:
            printf("Novembre\n");
            break;
        case 11:
            printf("Dicembre\n");
            break;
        default:
            printf("Numero non valido. Inserisci un numero da 0 a 11.\n");
            break;
    }



    time = mese*24*60*30;

    int energiaTotaleMensileGenerata = 0;

    int energiaMassimaMensile=0;
    int giornoEnergiaMassimaMensile=0;

    int energiaMinimaMensile=999999;
    int giornoEnergiaMinimaMensile=0;
    

    int maxPotenzaGenerata=0;
    int giornoPotenzaMax=0;
    int orarioEnMax=0;

    int minPotenzaGenerata=9999;
    int giornoEnergiaMIn=0;
    int orarioEnMin =0;
    for(int i=0;i<30;i++){
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
        std::cout << "] potenza max "<< maxEnergiaGiorno<< " alle ore "<< orarioMax;
        std::cout << "]  energia cumulata "<< energiaCumulata<<"\n";
        

        if(maxPotenzaGenerata<maxEnergiaGiorno){
            giornoPotenzaMax=i;
            maxPotenzaGenerata=maxEnergiaGiorno;
            orarioEnMax=orarioMax;
        }
        if(minPotenzaGenerata>maxEnergiaGiorno){
            minPotenzaGenerata=maxEnergiaGiorno;
            giornoEnergiaMIn=i;
            orarioEnMin=orarioMax;
        }
        if(energiaCumulata>=energiaMassimaMensile){
            energiaMassimaMensile=energiaCumulata;
            giornoEnergiaMassimaMensile=i;
        }
        if(energiaCumulata<=energiaMinimaMensile){
            energiaMinimaMensile=energiaCumulata;
            giornoEnergiaMinimaMensile=i;
        
        }
        energiaTotaleMensileGenerata += energiaCumulata;
    }


    
    double media_energetica= (double)energiaTotaleMensileGenerata/30;
   
    std::cout << " picco massimo energia  dell'anno "<<  maxPotenzaGenerata<<" nel giorno "<< giornoEnergiaMassimaMensile <<" alle ore "<< orarioEnMax <<" \n";
    std::cout << " picco minimo di energia  dell'anno "<<  minPotenzaGenerata<<" nel giorno "<< giornoEnergiaMinimaMensile <<" alle ore "<< orarioEnMin<<" \n";
    std::cout << " energia giornaleira massima dell'anno "<<  energiaMassimaMensile<<" nel giorno "<< giornoEnergiaMassimaMensile<<" \n";
    std::cout << " energia giornaliera minima dell'anno "<<  energiaMinimaMensile<<" nel giorno "<< giornoEnergiaMinimaMensile<<" \n";
    std::cout << " Media del mese "<<  media_energetica<<"\n";    
    return 0;
}

    
