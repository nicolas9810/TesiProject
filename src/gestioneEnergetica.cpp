#include "strategia.h"
#include "strategiaGreedy.h"
#include "previsione.h"
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



namespace bp = boost::process;




using namespace std;

bool giornoNuovo=false;
bool giornoUno=true;
bool flagInterrupt;
mutex mutexFlag;
condition_variable cv;

int interrupt[INTERRUPTS];
mutex mutexInter;
// mutex critical;
mutex host;
bool riconfigurare;
condition_variable riconf;

mutex cfgfilelock;

bool off;
condition_variable flagOff;

Strategia *currentStrategy;
StrategiaGreedy straGreedy;
StrategiaGreedyPredizione straGreedyPredizione;
StrategiaVirtualePredizione straVirtualePredizione;
StrategiaVirtuale straVirtuale;
int configurazioneAttuale;

epever *device;
//Tempo tempo;
Tempo *tim;

string cfgfilename = "../cfg/fmxcku115r1_3.cfg";
string comandoUp ="profpga_run "+ cfgfilename+" --up";
string comandoDown ="profpga_run "+ cfgfilename+" --down";
//string comand="/home/nicolas/Codice/TesiProject/prova3";

pthread_attr_t attr;
struct sched_param param;




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
    pthread_setname_np(pthread_self(), "Main Thread");
    //creazione della strategia
    tim=new Tempo(0,0,0,0);
    //tempo=Tempo();
    straGreedy=StrategiaGreedy(device);
   // straGreedyPredizione=StrategiaGreedyPredizione(device,tim);
    // straPredizioneSimul=StrategiaPredizioneSimul(device);

    straVirtualePredizione=StrategiaVirtualePredizione(device,tim);
    straVirtuale=StrategiaVirtuale(device,tim);

    currentStrategy=&straVirtuale;

    riconfigurare=true;
    inizializzaMatrice();



}

float getBatteryCharge(){
    return 90;//device->getBatterySOC();
}
float misuraPotenza(){
    return device->getArrayPower();
}

void* gestionePrevisioni(void* arg){
    pthread_setname_np(pthread_self(),"Gestione Previsioni");
    #ifdef DEBUG_MODE
     std::cout<<"[T GESTIONE PREVISIONI] gestisco le previsiomi"<<std::endl;
    #endif
    currentStrategy->gestionePrevisioni();

    return nullptr;

}


void* gestioneStrategia( void* arg){
    pthread_setname_np(pthread_self(),"Strategia");
    #ifdef DEBUG_MODE
   std::cout<<"[T GESTIONE ENERGETICA - gestioneEner] inzio a monitorare l'energia"<<std::endl;
    #endif
    int configurazione;
    float batteryStatus;
    while(true){
        batteryStatus = getBatteryCharge();
        #ifdef DEBUG_MODE
       //std::cout<<"[T GESTIONE ENERGETICA - gestioneEner] Batteria al: "<<batteryStatus<<"\%"<<std::endl;
        #endif
        //currentStrategy->setConsumi(configurazione);
        configurazione=currentStrategy->strategia(batteryStatus,configurazione);
        if (configurazione!=configurazioneAttuale){
            #ifdef DEBUG_MODE
           std::cout<<"Nuova configurazione scelta, attuale= "<<configurazioneAttuale<<", nuova= "<<configurazione<<std::endl;
            #endif
            unique_lock<std::mutex> lck1(mutexFlag);
            unique_lock<std::mutex> lck2(mutexInter);
            #ifdef DEBUG_MODE
           std::cout<<"[ T GESTIONE ENERGETICA - gestioneEner] modifico l'array di interrupt"<<std::endl;
            #endif
            interrupt[INTCONFIG]=configurazione;
            flagInterrupt=true;
            cv.notify_all();
        }
        else{
            #ifdef DEBUG_MODE
           std::cout<<"[ MAIN - GESTIONE STRATEGIA ]configurazione  adatta = "<<configurazioneAttuale<<std::endl;
            #endif
        }
        for(int i=0;i<3;i++){
             batteryStatus = getBatteryCharge();
            #ifdef DEBUG_MODE
           std::cout<<"[T GESTIONE ENERGETICA - gestioneEner] + Batteria al: "<<batteryStatus<<"\%"<<std::endl;
            #endif
            if(batteryStatus<30){
                unique_lock<std::mutex> lck1(mutexFlag);
                unique_lock<std::mutex> lck2(mutexInter);
                interrupt[INTSPEGNI]=configurazione;
                flagInterrupt = true;
                cv.notify_all();
            }
            #ifdef VIRTUALE
            this_thread::sleep_for(chrono::seconds(2));
            #endif


            #ifndef VIRTUALE
            //this_thread::sleep_for(chrono::minutes(FREQUENZA*2));

            #endif
        }

    }
    return nullptr;
}


void* gestioneHost(void* args){
    pthread_setname_np(pthread_self(), "Gestione Host");
    while(true){
        
        {unique_lock<std::mutex> lck(host);
        if(riconfigurare){
            riconf.wait(lck);
        }}
        {unique_lock<std::mutex> lck2(cfgfilelock);
        if(off){
            flagOff.wait(lck2);
        }}

        std::cout<< "[GESTIONE HOST ] eseguo hw"<<std::endl;
        string img1 = "../immagini/mont5.jpg";
        string out1 = "../immagin/outputhw.png";
        string out2 = "../immagini/outputsw.png";
        string exec = "../bin/main";
        
        string run = exec +" "+ cfgfilename +" " + img1 +" "+ out1 +" "+ out2;
        system(run.c_str());
        this_thread::sleep_for(chrono::seconds(2));
    }
    return nullptr;
}

//gestione Tempo
void* gestioneTempo(void* args){
    pthread_setname_np(pthread_self(), "Gestione Tempo");
    //tim->incrementsMinutes(700);
    while(true){
        
        tim->incrementsMinutes(10);
        //tempo.incrementa(40);
        #ifdef VIRTUALE
       std::cout << "[T GESTIONE TEMPO] sono passati 20 minuti, siamo al "<<tim->getTimeInMin()<<" minuto\n";
        #endif
        this_thread::sleep_for(chrono::seconds(3));
        //sleep(10);
    }
    return nullptr;
}

/* +++++++++++++++++++++ ROUTINE ++++++++++++++++++++++++++++++++*/

    void routineSpegni(int& interrupt){

        int exitcode = system(comandoDown.c_str());
        unique_lock<std::mutex> lck(mutexInter);
        interrupt =-1;

    }

    void routineCambioConfigurazione(int* interrupt)
    {
        #ifdef DEBUG_MODE
        std::cout<<"[MAIN - ROUT Cambio cfg] "<<*interrupt<<std::endl;
        #endif
        configurazioneAttuale=*interrupt;
        int exitcode;
        unique_lock<std::mutex> lck1(host);
        unique_lock<std::mutex> lck(cfgfilelock);

        switch (configurazioneAttuale)
        {
            case 0:
                #ifdef DEBUG_MODE
            std::cout<<"[MAIN - ROUT conf 0] "<<*interrupt<<std::endl;
                #endif
                
                //unique_lock<std::mutex> lck(cfgfilelock);
                off=true;
                exitcode = system(comandoDown.c_str());
                flagOff.notify_all();
                break;
            case 1:
                #ifdef DEBUG_MODE
            std::cout<<"[MAIN - ROUT conf 1] "<<*interrupt<<std::endl;
                #endif
                //unique_lock<std::mutex> lck(cfgfilelock);
                off=false;
                cfgfilename = "../cfg/fmxcku115r1_1.cfg";
                exitcode = system(comandoUp.c_str());
                flagOff.notify_all();
                
                break;
            case 2:
                #ifdef DEBUG_MODE
            std::cout<<"[MAIN - ROUT conf 2] "<<*interrupt<<std::endl;
                #endif
                //unique_lock<std::mutex> lck(cfgfilelock);
                off=false;
                cfgfilename = "../cfg/fmxcku115r1_2.cfg";
                exitcode = system(comandoUp.c_str());
                flagOff.notify_all();
                break;
            case 3:
                #ifdef DEBUG_MODE
            std::cout<<"[MAIN - ROUT conf 3] "<<*interrupt<<std::endl;
                #endif
                //unique_lock<std::mutex> lck(cfgfilelock);
                off=false;
                cfgfilename = "../cfg/fmxcku115r1_3.cfg";
                exitcode = system(comandoUp.c_str());
                flagOff.notify_all();
                break;
            default:
                #ifdef DEBUG_MODE
                std::cout<<"[MAIN - ROUT conf default] "<<*interrupt<<std::endl;
                #endif
                //unique_lock<std::mutex> lck(cfgfilelock);
                off=false;
                exitcode = system(comandoDown.c_str());
                flagOff.notify_all();
                break;
        }
        riconfigurare=true;
        riconf.notify_all();
        #ifdef DEBUG_MODE
       std::cout<<"[MAIN - ROUT sistema riconfigurato] "<<*interrupt<<std::endl;
        #endif

        unique_lock<std::mutex> lck2(mutexInter);
        *interrupt=-1;
        riconfigurare=false;

    }
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


void* runFpga(void* arg){
    int* intPtr = static_cast<int*>(arg);
    pthread_setname_np(pthread_self(), "RunFpga");
    #ifdef DEBUG_MODE
   std::cout<<"[RUN FPGA] configuro fpga"<<std::endl;
    #endif
    routineCambioConfigurazione(intPtr);
   
    
    return nullptr;
}


void checkInterrupt(){
    for(int i =0;i<INTERRUPTS;i++){
        unique_lock<std::mutex> lck(mutexInter);
        if(interrupt[i]!=-1){
            switch (i)
            {
            case 0:
            #ifdef DEBUG_MODE
               std::cout<<"[MAIN - interrupt] spegni"<<std::endl;
                #endif
                routineSpegni(interrupt[i]);
                break;
            case 1:
                #ifdef DEBUG_MODE
               std::cout<<"[MAIN - interrupt] cambio config"<<std::endl;
                #endif
                pthread_t highPriorityThread1;
                int result = pthread_create(&highPriorityThread1, &attr, runFpga, &interrupt[i]);
                if (result != 0) {
                    std::cerr << "Errore nella creazione del thread ad alta priorità" << std::endl;
                    break;
                }
                // routineCambioConfigurazione(interrupt[i]);
            }
        }
    }
}


int main(){


    // pthread_t highPriorityThread1;
    // pthread_t highPriorityThread2;

    pthread_t normalThread1;
    pthread_t normalThread2;
    pthread_t normalThread3;
    pthread_t normalThread4;

   

    // pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_attr_setschedparam(&attr, &param);

    // system(comandoUp.c_str());
    // string run ="/tools/prodesign/profpga/proFPGA-2020D-SP2/hdl/demo_designs/mmi64_basic/session/gcc/main " + cfgfilename;
    // system(run.c_str());
    // this_thread::sleep_for(chrono::seconds(5));


    inizializzazione();

    

    int result = pthread_create(&normalThread1, nullptr, gestioneTempo, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
        return 1;
    }
    // result = pthread_create(&normalThread2, nullptr, gestionePrevisioni, nullptr);
    // if (result != 0) {
    //     std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
    //     return 1;
    // }
     // Crea un thread con priorità normale
    result = pthread_create(&normalThread3, nullptr, gestioneStrategia, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
        return 1;
    }
    result = pthread_create(&normalThread4, &attr, gestioneHost, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread gestione host " << std::endl;
        return 1;
    }

    while(true){
        
        #ifdef DEBUG_MODE
       std::cout<<"[MAIN] thread controlla gli interrupt"<<std::endl;
        #endif
        unique_lock<std::mutex> lck(mutexFlag);
        if(!flagInterrupt){
            cv.wait(lck);
        }
        checkInterrupt();
        flagInterrupt=false;
    }
    return 0;
}

