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
Tempo *tim;

string cfgfilename = "/home/nicolas/Codice/TesiProject/cfg/fmxcku115r1_3.cfg";
string comandoUp ="profpga_run "+cfgfilename+" --up";
string comandoDown ="profpga_run "+cfgfilename+" --down";
string comand="/home/nicolas/Codice/TesiProject/prova3";




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
    // pthread_setname_np(pthread_self(), "Main Thread");
    //creazione della strategia
    tim=new Tempo(0,0,0,0);
    //tempo=Tempo();
    straGreedy=StrategiaGreedy(device);
    straGreedyPredizione=StrategiaGreedyPredizione(device,tim);
    // =StrategiaPredizioneSimul(device);
    //straVirtualePredizione=StrategiaVirtualePredizione(device,&tim);
    straVirtuale=StrategiaVirtuale(device,tim);

    currentStrategy=&straVirtuale;
    //string comand="/home/nicolas/Codice/TesiProject/prova";
    //int exitcode = system(comand.c_str());
    //#ifdef DEBUG_MODE
    //cout<<"[ INIT - EXIT CODE ] :"<<exitcode<<endl;
    //#endif 

    //connessione dispositivo
    // scegliere il nome della porta adeguato
    

}

float getBatteryCharge(){
    return 90;//device->getBatterySOC();
}
float misuraPotenza(){
    return device->getArrayPower();
}
int strategia(int batteryCharge){
    float previsioneEnergia = previsioneEnergiaDisponibile(PREDWINDOW,min);
    if(batteryCharge <30){
        return OFF;
    }
    else if(batteryCharge>= 30 && batteryCharge <50){
        int flag = getBestConfig(previsioneEnergia);
        if(flag==-1){
            return MINPOWER;
        }
        else{
            return flag;
        }

    }
    else if(batteryCharge>= 50 && batteryCharge<70){
        int flag = getBestConfig(previsioneEnergia);
        if(flag==-1){
            return MEDPOWER;
        }
        else{
            return flag;
        }
    }
    else{
        return MAXPOWER;
    }
}
void gestioneStrategia(){
    #ifdef DEBUG_MODE
    cout<<"[T GESTIONE ENERGETICA - gestioneEner] inzio a monitorare l'energia"<<endl;
    #endif    
    int configurazione;
    float batteryStatus;

    batteryStatus = getBatteryCharge();
    #ifdef DEBUG_MODE
    cout<<"[T GESTIONE ENERGETICA - gestioneEner] Batteria al: "<<batteryStatus<<"\%"<<endl;
    #endif
    //currentStrategy->setConsumi(configurazione);
    configurazione=currentStrategy->strategia(batteryStatus,configurazione);
    if (configurazione!=configurazioneAttuale){   
        #ifdef DEBUG_MODE
        cout<<"Nuova configurazione scelta, attuale= "<<configurazioneAttuale<<", nuova= "<<configurazione<<endl;
        #endif
        unique_lock<std::mutex> lck1(mutexFlag);
        unique_lock<std::mutex> lck2(mutexInter);
        #ifdef DEBUG_MODE
        cout<<"[ T GESTIONE ENERGETICA - gestioneEner] modifico l'array di interrupt"<<endl;
        #endif
        interrupt[INTCONFIG]=configurazione;
        flagInterrupt=true;
        cv.notify_all();            
        
    }  
    else{
        #ifdef DEBUG_MODE
        cout<<"[ MAIN - GESTIONE STRATEGIA ]configurazione è adatta = "<<configurazioneAttuale<<endl;
        #endif
    }
    for(int i=0;i<3;i++){
        batteryStatus = getBatteryCharge();
        #ifdef DEBUG_MODE
        //cout<<"[T GESTIONE ENERGETICA - gestioneEner] + Batteria al: "<<batteryStatus<<"\%"<<endl;
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
    this_thread::sleep_for(chrono::minutes(FREQUENZA*2));
    #endif      
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
//             if(ultimoSlot==NN/2 && !giornoUno){
//                 previsioneDelGiorno(ultimoSlot);
//                 #ifdef DEBUG_MODE
//                 cout<<"È mezzogiorno, riaggiorno le previsioni"<<endl;
//                 #endif   
//             }
//             if(ultimoSlot>=NNN-1){
//                 #ifdef DEBUG_MODE
//                 cout<<"È l'ultimo slot del giorno, pronto a passare al prossimogiorno"<<endl;
//                 #endif
//                giornoNuovo=true;
//                if(giornoUno)giornoUno=false;
//             }         
//         }
//     }    
// }

void* gestioneHost(void* args){
    return nullptr;
}
//gestione Tempo
void* gestioneTempo(void* args){
    pthread_setname_np(pthread_self(), "Gestione Tempo");
    while(true){
    tim->incrementsMinutes(45);
    //tempo.incrementa(40);
    #ifdef VIRTUALE
    cout << "[T GESTIONE TEMPO] sono passati 5 minuti, siamo al "<<tim->getTimeInMin()<<" minuto\n";
    #endif
    this_thread::sleep_for(chrono::seconds(2));
    //sleep(10);
    }
    return nullptr
}

/* +++++++++++++++++++++ ROUTINE ++++++++++++++++++++++++++++++++*/

    void routineSpegni(int& interrupt){

    /*
    
    TODO
    devo fare:
    profpga_run configurazione --down
    configurazione = null;
    */

        int exitcode = system(comandoDown.c_str());
        interrupt =-1;
    }

    void routineCambioConfigurazione(int &interrupt){
        /*
        TODO
        devo fare:
        profpga_run configurazione --down
        profpga_run nuova configurazione --up
        configurazione = nuovaconfigurazione;
        */
        

        #ifdef DEBUG_MODE
        cout<<"[MAIN - ROUT Cambio cfg] "<<interrupt<<endl;
        #endif
        configurazioneAttuale=interrupt;
        int exitcode;
        switch (configurazioneAttuale)
        {
        case 0:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 0] "<<interrupt<<endl;
            #endif
            exitcode = system(comandoDown.c_str());
            break;
        case 1:
        #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 1] "<<interrupt<<endl;
            #endif
            cfgfilename = "/home/nicolas/Codice/TesiProject/cfg/fmxcku115r1_1.cfg";
            exitcode = system(comandoUp.c_str());
            break;
        case 2:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 2] "<<interrupt<<endl;
            #endif
            cfgfilename = "/home/nicolas/Codice/TesiProject/cfg/fmxcku115r1_2.cfg";
            exitcode = system(comandoUp.c_str());
            break;
        case 3:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 3] "<<interrupt<<endl;
            #endif
            cfgfilename = "/home/nicolas/Codice/TesiProject/cfg/fmxcku115r1_3.cfg";
            exitcode = system(comandoUp.c_str());
            break;
        default:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf default] "<<interrupt<<endl;
            #endif
            exitcode = system(comandoDown.c_str());
            break;
        }

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

void runFpga(){
    //pthread_setname_np(pthread_self(), "Run");
    string cfgfilename = "/home/nicolas/Codice/TesiProject/cfg/fmxcku115r1.cfg";
    string comando ="/home/nicolas/Codice/TesiProject/prova";
    try {
        bp::system(comando);  // Esegue il comando
    } catch (const std::exception &e) {
        std::cerr << "Errore durante l'esecuzione del comando: " << e.what() << "\n";
    }
    cout<<"[MAIN] sezione critica termianta"<<endl;
    sleep(3);
    
    return;
}

int main(){

    pthread_t normalThread1;
    pthread_t normalThread2;


    
    int c =0;
    
    
    #ifdef DEBUG_MODE
    cout<<"[MAIN] Inizializzando il sistema"<<endl;
    #endif

    for(int i=0;i<INTERRUPTS;i++){
        interrupt[i]=-1;
    }
    // pthread_setname_np(pthread_self(), "Main Thread");
    //creazione della strategia
    
    inizializzazione();
    int result = pthread_create(&normalThread1, nullptr, gestioneTempo, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
        return 1;
    }
    result = pthread_create(&normalThread1, nullptr, gestioneHost, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
        return 1;
    }

    while(c<100){
        
        //runFpga();
       

        #ifdef VIRTUALE
        cout<<"[MAIN] creo thread che gestisce la strategia"<<endl;
        #endif
        gestioneStrategia();

        #ifdef DEBUG_MODE
        cout<<"[MAIN] creo thread che gestisce l'host"<<endl;
        #endif
        gestionePrevisioni();
        // //2. thread che gestice la comunicazione con l'host
        // exitcode = system(comand.c_str());
        #ifdef DEBUG_MODE
        cout<<"[MAIN] thread controlla gli interrupt"<<endl;
        #endif
        //unique_lock<std::mutex> lck(mutexFlag);
        // if(!flagInterrupt){
        //     cv.wait(lck);
        // }
        checkInterrupt();
        flagInterrupt=false;
        c++;
    }

    #ifdef DEBUG_MODE
        cout<<"[MAIN] fine"<<endl;
        #endif
    return 0;
}

