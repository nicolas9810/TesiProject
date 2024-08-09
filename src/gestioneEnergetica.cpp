#include "strategia.h"
#include "strategiaGreedy.h"
#include "strategiaGreedyPredizione.h"
#include "strategiaVirtualePredizione.h"
#include "strategiaVirtuale.h"
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include "defines.h"
#include <pthread.h>
#include <string>
#include "tempo.h"
#include <boost/process.hpp>
#include <pthread.h>
#include "previsione.h"
#include "batteria.h"
/*
TODO 
Provare a inserire la strategia direttamente qui per vedere se riesco a prendere il tempo

*/

namespace bp = boost::process;




using namespace std;

bool giornoNuovo=false;
bool giornoUno=true;
bool flagInterrupt;
mutex mutexFlag;
condition_variable cv;




bool off=true;
condition_variable flagOff;

int interrupt[INTERRUPTS];
mutex mutexInter;

Strategia *currentStrategy;
StrategiaGreedy straGreedy;
StrategiaGreedyPredizione straGreedyPredizione;
StrategiaVirtualePredizione straVirtualePredizione;
StrategiaVirtuale straVirtuale;
int configurazioneAttuale;

epever *device;
//Tempo tempo;
mutex tempo;
condition_variable cvtempo;
Tempo *tim;
int giornoAttuale;
float energiaDomani;

int tempoUltimaMisurazione;
int slotAttuale;
float EnergiaPrevista;
float EnergiaEffettivaAccumulata;
float gap;
GreenPlantModel *modelloPannello;
Batteria *batteria;

string cfgfilename = "../cfg/fmxcku115r1_1.cfg";
//string comandoUp ="profpga_run "+cfgfilename+" --up";
// string comandoDown ="profpga_run "+cfgfilename+" --down";





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

void inizializzazione(){
    device = new epever("/dev/ttyXRUSB0");
    for(int i=0;i<INTERRUPTS;i++){
        interrupt[i]=-1;
    }
    modelloPannello=new GreenPlantModel("../csv/2018.csv");
    // pthread_setname_np(pthread_self(), "Main Thread");
    //creazione della strategia
    tim=new Tempo(90,8,0,0);
    giornoAttuale=90;
    //tempo=Tempo();
    tempoUltimaMisurazione=tim->getTimeInMin();
    //Batteria ( Energia Max, efficienza, coef di peridite, limite di scaricamento, finestra di predizione)
    batteria=new Batteria(ENMAX,EFFICIENCY,LOSS,MINIMUMCHARGE,FINESTRAPREDIZIONE);
   
    //straGreedy=StrategiaGreedy(device);
    //straGreedyPredizione=StrategiaGreedyPredizione(device,tim);
    // =StrategiaPredizioneSimul(device);
    //straVirtualePredizione=StrategiaVirtualePredizione(device,&tim);
    //straVirtuale=StrategiaVirtuale(device,tim);

    //currentStrategy=&straVirtuale;
    //string comand="/home/nicolas/Codice/TesiProject/prova";
    //int exitcode = system(comand.c_str());
    

}

float getBatteryCharge(){
    return 90;//device->getBatterySOC();
}
float misuraPotenza(){
    return device->getArrayPower();
}

float previsione(int t){
    
    //ritornare energia nella finestra di tempo successiva con una percentaule di errore del 20/15%*/
    float energiaInArrivo=0;
    for(int i= t;i< t+FINESTRA;i++){
        energiaInArrivo+= modelloPannello->getProducedPowerByTime(i);
    }
    float erroreRandom= static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    
    // Scala il valore nel range -0.20 a 0.20
    float en= energiaInArrivo*((erroreRandom * 0.40f) - 0.20f);
    #ifdef DEBUG_MODE
    cout<<"[previsione -] energia prevista "<< (energiaInArrivo+en)/60 <<" energia vera "<< energiaInArrivo/60<<"\n";
    #endif
    batteria->setEnergiaPrevista((energiaInArrivo+en)/60);
    return (energiaInArrivo+en)/60;
}

float GAP(){
    float energia =EnergiaEffettivaAccumulata- EnergiaPrevista;
    #ifdef DEBUG_MODE
    cout<<"[ GAP ] EnergiaPrevista "<< EnergiaPrevista <<" - EnergiaEffettivaAccumulata" << EnergiaEffettivaAccumulata<<"\n";
    #endif
    return energia;
}

float consumi[]={40,89,120,150};




int configurazione(){
    int z = energiaPrevistaDomani()/batteria->getEnergiaMax();
    if(z>1){
        z=1;
    }
    int tau = tempoProssimaRicarica(tim->getTimeInMin());
    float c= consumi[0]*tau;
    float energiaBatt=batteria->getEnergia();
    if(z*energiaBatt-c>0){
        int i=3;
        bool flag=false;
         while(i>0 && !flag){
            
            float consumo=(consumi[i]/60)*FINESTRA;
            float proiezione=EnergiaPrevista-consumo+energiaBatt;
            #ifdef DEBUG_MODE
            std::cout<<"[Cerco conf -] conf = "<< i <<", previsione"<<proiezione<<std::endl;
            #endif
            if(proiezione > c){
                flag=true;
                break;
            }
            i--;
        }
        return i;
    }
    return 0;
}
float consumo(int t){
    return batteria->energiaConsumata(consumi[configurazioneAttuale],(t-tempoUltimaMisurazione));
}

float energiaArrivata(int t){
    
    float energiaInput=0;
    for(int i= tempoUltimaMisurazione;i< t;i++){
        energiaInput+= modelloPannello->getProducedPowerByTime(i);
    }
    std::cout<<"[ENERGIA ARRIVATA ="<< energiaInput/60<<" che diventa "<<(energiaInput/60)*0.8<<std::endl;

    return batteria->setEnergiaAccumulata(energiaInput/60);
}

float energiaPrevistaDomani(){
    float energia=0;
    int minuto=(giornoAttuale+1)*24*60;
    for(int i=0;i<1440;i+=60){
        energia+=modelloPannello->getProducedPowerByTime(minuto+i);
    }
    return energia;
}
int tempoProssimaRicarica(int t){
    int tau =0;
    return tau;
}

int strategia(){
    int soc=batteria->SoC();
    if(soc==0){
        cout<<"BATTERIA ALLO 0%\n";
    }

   

    int tempoAttuale;
    unique_lock<std::mutex> lck(tempo);
    tempoAttuale=tim->getTimeInMin();

    


    #ifdef DEBUG_MODE
    cout<<"[STRATEGIA -] tempo Attuale "<< tempoAttuale<<" ultima misurazione "<<tempoUltimaMisurazione<<endl;
    #endif
    float EnergiaConsumata=consumo(tempoAttuale);
    #ifdef DEBUG_MODE
    cout<<"[STRATEGIA -] energia consumata"<< EnergiaConsumata<<"\n";
    #endif
    EnergiaEffettivaAccumulata=energiaArrivata(tempoAttuale);
    batteria->chargeDischarge();
    soc=batteria->SoC();
    cout<<"[ STRATEGIA -] BATTERIA ALLO "<<soc<<"%\n";

    if(soc==0){
        cout<<"BATTERIA ALLO 0%\n";
    }
    //gap = GAP();
    #ifdef DEBUG_MODE
    cout<<"[STRATEGIA -] energia Accumulata "<< EnergiaEffettivaAccumulata<<" con Gap "<< gap<<"\n";
    #endif
    EnergiaPrevista = previsione(tempoAttuale);
    if(EnergiaPrevista==0){
        #ifdef DEBUG_MODE
        cout<<"[STRATEGIA -] NIENTE LUCE NELLA PROSSIMA FINESTRA\n";
        #endif
    }
    #ifdef DEBUG_MODE
    cout<<"[STRATEGIA -] energiaPrevista "<< EnergiaPrevista<<"\n";
    #endif
    int finestreTemporale=tempoAttuale-tempoUltimaMisurazione;
    int i= configurazione(finestreTemporale);
    tempoUltimaMisurazione=tim->getTimeInMin();
    
    #ifdef DEBUG_MODE
    cout<<"[STRATEGIA -] scelta configurazione "<< i <<"\n";
    #endif

    return i;
    
}
void gestioneStrategia(){
    #ifdef DEBUG_MODE
    cout<<"[T GESTIONE ENERGETICA - gestioneEner] inzio a monitorare l'energia"<<endl;
    #endif    
    int configurazione;
    float batteryStatus;

    batteryStatus = batteria->SoC();
    #ifdef DEBUG_MODE
    cout<<"[T GESTIONE ENERGETICA - gestioneEner] Batteria al: "<<batteryStatus<<"\%"<<endl;
    #endif
    //currentStrategy->setConsumi(configurazione);
    configurazione=strategia();
    if (configurazione!=configurazioneAttuale){   
        #ifdef DEBUG_MODE
        cout<<"Nuova configurazione scelta, attuale= "<<configurazioneAttuale<<", nuova= "<<configurazione<<endl;
        #endif
        
        interrupt[INTCONFIG]=configurazione;
        flagInterrupt=true;             
    }  
    else{
        #ifdef DEBUG_MODE
        cout<<"[ MAIN - GESTIONE STRATEGIA ]configurazione è adatta = "<<configurazioneAttuale<<endl;
        #endif
    }
    // for(int i=0;i<3;i++){
    //     batteryStatus = getBatteryCharge();
    //     #ifdef DEBUG_MODE
    //     //cout<<"[T GESTIONE ENERGETICA - gestioneEner] + Batteria al: "<<batteryStatus<<"\%"<<endl;
    //     #endif
    //     if(batteryStatus<30){      
    //         interrupt[INTSPEGNI]=configurazione;
    //         flagInterrupt = true;          
    //     }
    // #ifdef VIRTUALE
    // this_thread::sleep_for(chrono::seconds(2));
    // #endif
    // #ifndef VIRTUALE
    // this_thread::sleep_for(chrono::minutes(FREQUENZA*2));
    // #endif      
    // }
  
}




void gestioneHost(){
    
        std::cout<< "[GESTIONE HOST ] eseguo hw"<<std::endl;
        string img1 = "../immagini/mont5.jpg";
        string out1 = "../immagin/outputhw.png";
        string out2 = "../immagini/outputsw.png";
        string exec = "../bin/main";
        
        string run = exec +" "+ cfgfilename +" " + img1 +" "+ out1 +" "+ out2;
        system(run.c_str());
        this_thread::sleep_for(chrono::seconds(2));
}
//gestione Tempo
void* gestioneTempo(void* args){
    pthread_setname_np(pthread_self(), "Gestione Tempo");
    while(true){
    {
        unique_lock<std::mutex> lck(tempo);
        tim->incrementsMinutes(1);
    }

    int minuti=tim->getMinutes();
    int ore=tim->getHours();
    int giorno=tim->getDay();
    if(giorno!=giornoAttuale){
        giornoAttuale=giorno;
        energiaDomani=energiaPrevistaDomani;
    }
  
    #ifdef VIRTUALE
    cout << "[T GESTIONE TEMPO] sono passati minuti, è il giorno "<<giorno<<" e le ore"<<ore<<":"<<minuti<<"\n";
    #endif
    this_thread::sleep_for(chrono::milliseconds(200));
  
    }
    return nullptr;
}

/* +++++++++++++++++++++ ROUTINE ++++++++++++++++++++++++++++++++*/

    void routineSpegni(int& interrupt){
        string comandoDown ="profpga_run "+cfgfilename+" --down";
        int exitcode = system(comandoDown.c_str());
        interrupt =-1;
    }

    void routineCambioConfigurazione(int &interrupt){
        #ifdef DEBUG_MODE
        cout<<"[MAIN - ROUT Cambio cfg] "<<interrupt<<endl;
        #endif
        configurazioneAttuale=interrupt;
        int exitcode;
        string comandoDown;
        string comandoUp;
        switch (configurazioneAttuale)
        {
        case 0:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 0] "<<interrupt<<endl;
            #endif
            off=true;
            comandoDown ="profpga_run "+cfgfilename+" --down";
            //exitcode = system(comandoDown.c_str());
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 0 down] "<<cfgfilename<<endl;
            #endif
            break;
        case 1:
        #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 1] "<<interrupt<<endl;
            #endif
            off=false;
            cfgfilename = "../cfg/fmxcku115r1_1.cfg";
            comandoUp ="profpga_run "+cfgfilename+" --up";
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 1] "<<cfgfilename<<endl;
            #endif
            //exitcode = system(comandoUp.c_str());
            
            break;
        case 2:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 2] "<<interrupt<<endl;
            #endif
            off=false;
            cfgfilename = "../cfg/fmxcku115r1_2.cfg";
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 2] "<<cfgfilename<<endl;
            #endif
            comandoUp ="profpga_run "+cfgfilename+" --up";
            //exitcode = system(comandoUp.c_str());
            
            break;
        case 3:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 3] "<<interrupt<<endl;
            #endif
            off=false;
            cfgfilename = "../cfg/fmxcku115r1_3.cfg";
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 3] "<<cfgfilename<<endl;
            #endif
            comandoUp ="profpga_run "+cfgfilename+" --up";
            //exitcode = system(comandoUp.c_str());
            
            break;
        default:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf default] "<<interrupt<<endl;
            #endif
            off=true;
            comandoDown ="profpga_run "+cfgfilename+" --down";
            //exitcode = system(comandoDown.c_str());
            
            break;
        }
        

        interrupt=-1;

    }
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/




void checkInterrupt(){
        for(int i =0;i<INTERRUPTS;i++){
            if(interrupt[i]!=-1){
                switch (i)
                {
                case 0:
                #ifdef DEBUG_MODE
                    cout<<"[MAIN - interrupt] spegni"<<endl;
                    #endif
                    routineSpegni(interrupt[i]);
                    break;
                case 1:
                    #ifdef DEBUG_MODE
                    cout<<"[MAIN - interrupt] cambio config"<<endl;
                    #endif
                    routineCambioConfigurazione(interrupt[i]);
                    break;
                default:
                    break;
                }
            }

        }
}


int main(){

    pthread_t normalThread1;
    pthread_t normalThread2;

   

    
    int c =0;
    
    
    #ifdef DEBUG_MODE
    cout<<"[MAIN] Inizializzando il sistema"<<endl;
    #endif

    inizializzazione();
    int result = pthread_create(&normalThread1, nullptr, gestioneTempo, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
        return 1;
    }
    #ifdef DEBUG_MODE
    cout<<"[MAIN] creo thread che gestisce l'host"<<endl;
    #endif

    EnergiaPrevista=modelloPannello->getProducedPowerByTime(tim->getTimeInMin())*FINESTRAPREDIZIONE/60;
    #ifdef DEBUG_MODE
    cout<<"[MAIN] Predizione iniziale "<< EnergiaPrevista<<endl;
    #endif
    while(true){
        int tempoAttuale;
        {
            unique_lock<std::mutex> lck(tempo);
            tempoAttuale=tim->getTimeInMin();
        }
        if(tempoAttuale>=tempoUltimaMisurazione+FINESTRAPREDIZIONE)
        {
            #ifdef DEBUG_MODE
            cout<<"[MAIN] Cerco strategia nuova"<<endl;
            #endif
            gestioneStrategia();
        }else if(!off){
            //gestioneHost();
        }

        
        // //2. thread che gestice la comunicazione con l'host
        // exitcode = system(comand.c_str());
        
        if(flagInterrupt){
            #ifdef DEBUG_MODE
            cout<<"[MAIN] controllo interrupt"<<endl;
            #endif
            checkInterrupt();
            flagInterrupt=false;
        }
    }

    #ifdef DEBUG_MODE
        cout<<"[MAIN] fine"<<endl;
        #endif
    return 0;
}

