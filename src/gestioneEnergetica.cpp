#include "strategia.h"
#include "strategiaGreedy.h"
#include "strategiaGreedyPredizione.h"
#include "strategiaPredizioneSimul.h"
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include "defines.h"



using namespace std;

bool giornoNuovo=false;
bool giornoUno=true;
bool flagInterrupt;
mutex mutexFlag;
condition_variable cv;

int interrupt[INTERRUPTS];
mutex mutexInter;
Strategia *currentStrategy;
StrategiaGreedy straGreedy;
StrategiaGreedyPredizione straGreedyPredizione;
StrategiaPredizioneSimul straPredizioneSimul;
int configurazioneAttuale;

epever *device;



class CodaCircolare{
    private:
        int maxSize;
        int testa;
        bool full;
        float* array;
    public:
        

    CodaCircolare(int m){
        maxSize=m;
        array = new float[m];
        testa=0;
        full=false;
    }
    void addElement(float e){
        array[testa]=e;
        testa=(testa+1)%maxSize;
        if(testa==0){
            full=true;
        }
    }

    void azzera(){
        for(int i=0;i<testa;i++){
                array[i]=0;
        }
        full=false;
        testa=0;
    }
    float getMedia(){
        float media=0;
        if(!full){
            int c=0;
            for(int i=0;i<testa;i++,c++){
                media+=array[i];
                
            }
            return media/c;
        }
        else{
            for(int i=0;i<maxSize;i++){
                media+=array[i];
            }
            return media/maxSize;
        }
    }
};

void inizializzazione(epever* dev){
    for(int i=0;i<INTERRUPTS;i++){
        interrupt[i]=-1;
    }
    //creazione della strategia
    straGreedy=StrategiaGreedy(dev);
    straGreedyPredizione=StrategiaGreedyPredizione(dev);
    straPredizioneSimul=StrategiaPredizioneSimul(dev);
    currentStrategy=&straGreedy;

    //connessione dispositivo
    // scegliere il nome della porta adeguato
    device = new epever("/dev/ttyUSB0");

}

float getBatteryCharge(){
    return device->getBatterySOC();
}
float misuraPotenza(){
    return device->getArrayPower();
}


void gestioneStrategia(){
    int configurazione;
    float batteryStatus;
    while(true){
        batteryStatus = getBatteryCharge();
        #ifdef DEBUG_MODE
        cout<<"Batteria al: "<<batteryStatus<<"\%"<<endl;
        #endif
        currentStrategy->setConsumi(configurazione);
        configurazione=currentStrategy->strategia(batteryStatus);
        if (configurazione!=configurazioneAttuale){   
            #ifdef DEBUG_MODE
            cout<<"Nuova configurazione scelta, attuale= "<<configurazioneAttuale<<", nuova= "<<configurazione<<endl;
            #endif
            unique_lock<std::mutex> lck1(mutexFlag);
            unique_lock<std::mutex> lck2(mutexInter);
            interrupt[INTCONFIG]=configurazione;
            flagInterrupt=true;
            cv.notify_all();            

           
        }  
        for(int i=0;i<3;i++){
            batteryStatus = getBatteryCharge();
            if(batteryStatus<30){
                unique_lock<std::mutex> lck1(mutexFlag);
                unique_lock<std::mutex> lck2(mutexInter);
                interrupt[INTSPEGNI]=configurazione;
                flagInterrupt = true;
                cv.notify_all();            
            }
            this_thread::sleep_for(chrono::minutes(FREQUENZA*2));
        }
        
    }
}

// void gestionePrevisioni(){
//     CodaCircolare codaMisurazioni=CodaCircolare(5);
//     inizializzaMatrice();
//     int ultimoSlot=0;
//     while(true){
//         /*
//         se inizia un nuovo giorno devo fare uno shift verso sinistra di tutta la matrice e rifare le previsioni per il giorno
//         */
//         if(giornoNuovo){
//             #ifdef DEBUG_MODE
//             cout<<"È iniziato un nuovo giorno, calcolo le nuove previsioni"<<endl;
//             #endif
//            nuovoGiorno();
//            previsioneDelGiorno(0);
//            giornoNuovo=false;
//         }
//         /*
//         altrimenti faccio un minimo di 5 misurazioni ogni (FREQUENZA*60/15) secondi e calcolo la media che inserisco nella matrice energetica
//         */
//         else{
//             float potenzaInIngresso=misuraPotenza();
//             //effettuo una misurazione ogni FREQUENZA/10 min ed ogni FREQUENZA/2 prendo la media delle ultime 5 misruazioni
//             // e la inseirsco nella matrice delle misurazioni per aggiornare le previsioni
//             for(int i=0;i<5;i++){
//                 #ifdef DEBUG_MODE
//                 cout<<"Misura potenza in ingresso: "<<potenzaInIngresso<<endl;
//                 #endif
//                 codaMisurazioni.addElement(potenzaInIngresso);
//                 this_thread::sleep_for(chrono::minutes(FREQUENZA/10));
//             }
//             float mediaMisurazioni=codaMisurazioni.getMedia();
//             ultimoSlot=setEnergia(mediaMisurazioni);
//             #ifdef DEBUG_MODE
//             cout<<"slot del giorno: "<<ultimoSlot<<" = "<<media<<endl;
//             #endif
//             if(ultimoSlot==N/2 && !giornoUno){
//                 previsioneDelGiorno(ultimoSlot);
//                 #ifdef DEBUG_MODE
//                 cout<<"È mezzogiorno, riaggiorno le previsioni"<<endl;
//                 #endif   
//             }
//             if(ultimoSlot>=N-1){
//                 #ifdef DEBUG_MODE
//                 cout<<"È l'ultimo slot del giorno, pronto a passare al prossimogiorno"<<endl;
//                 #endif
//                giornoNuovo=true;
//                if(giornoUno)giornoUno=false;
//             }         
//         }
//     }    
// }

void gestioneHost(){

}

/* +++++++++++++++++++++ ROUTINE ++++++++++++++++++++++++++++++++*/

void routineSpegni(int& interrupt){


    /*
    
    TODO
    devo fare:
    profpga_run configurazione --down
    configurazione = null;
    */
    interrupt =-1;
}

void routineCambioConfigurazione(int& interrupt){
    /*
    TODO
    devo fare:
    profpga_run configurazione --down
    profpga_run nuova configurazione --up
    configurazione = nuovaconfigurazione;
    */
    interrupt=-1;

}
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/




void checkInterrupt(){
        unique_lock<std::mutex> lck(mutexInter);
        for(int i =0;i<INTERRUPTS;i++){
            if(interrupt[i]!=-1){
                switch (i)
                {
                case 0:
                    routineCambioConfigurazione(interrupt[i]);
                    break;
                case 1:
                    routineSpegni(interrupt[i]);
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    break;
                case 5:
                    break;
                default:
                    break;
                }
            }

        }
}
int main(){
    // 1.thread che monitora la batteria e calcola la strategia
    std::thread monitoraggioBatteria(gestioneStrategia);



    //2. thread che gestice la comunicazione con l'host
    std::thread host(gestioneHost);

    while(true){
        unique_lock<std::mutex> lck(mutexFlag);
        if(!flagInterrupt){
            cv.wait(lck);
        }
        checkInterrupt();
        flagInterrupt=false;
    }
    return 0;
}

