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

mutex host;
bool riconfigurare;
condition_variable riconf;

mutex cfgfilelock;

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

int tempoUltimaMisurazione;
int slotAttuale;
float EnergiaPrevista;
float EnergiaEffettivaAccumulata;
float gap;
GreenPlantModel *modelloPannello;

string cfgfilename = "../cfg/fmxcku115r1_3.cfg";
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
    modelloPannello=new GreenPlantModel("../csv/2018.csv");
    // pthread_setname_np(pthread_self(), "Main Thread");
    //creazione della strategia
    tim=new Tempo(0,7,30,0);
    //tempo=Tempo();
    tempoUltimaMisurazione=tim->getTimeInMin();
    straGreedy=StrategiaGreedy(device);
    //straGreedyPredizione=StrategiaGreedyPredizione(device,tim);
    // =StrategiaPredizioneSimul(device);
    //straVirtualePredizione=StrategiaVirtualePredizione(device,&tim);
    straVirtuale=StrategiaVirtuale(device,tim);

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
    /* TODO 
    
    ritornare energia nella finestra di tempo successiva con una percentaule di errore del 20/15%*/
    float energiaInArrivo=0;
    for(int i= t;i< t+FINESTRAPREDIZIONE;i++){
        energiaInArrivo+= modelloPannello->getProducedPowerByTime(i);
    }
    float erroreRandom= static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    
    // Scala il valore nel range -0.20 a 0.20
    float en= energiaInArrivo*((erroreRandom * 0.40f) - 0.20f);
    #ifdef DEBUG_MODE
    cout<<"[previsione -] energia prevista "<< energiaInArrivo+en <<" energia vera "<< energiaInArrivo<<"\n";
    #endif
    return energiaInArrivo+en;
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
    float delta;
    for(int i=MAXPOWER;i>=OFF;i--){
        delta=EnergiaPrevista-consumi[i]*FINESTRAPREDIZIONE + gap;
        #ifdef DEBUG_MODE
        cout<<"[Cerco conf -] delta = "<< delta <<" = Energia Prevista "<< EnergiaPrevista<<" - " <<consumi[i]*FINESTRAPREDIZIONE<<"(consumi "<<i<<") + "<<gap<<"\n";
        #endif
        //se trovo una configurazione che rientra
        if(delta>0){
            return i;
        }
    }
    //se non trovo niente, significa che non può caricarsi in questo momento
    return 0;
}
float consumo(int t){
    return consumi[configurazioneAttuale]*(t-tempoUltimaMisurazione);
}

float energiaArrivata(int t){
    
    float energiaInput=0;
    for(int i= tempoUltimaMisurazione;i< t;i++){
        energiaInput+= modelloPannello->getProducedPowerByTime(i);
    }
    return energiaInput;
}

int strategia(){
    int tempoAttuale;
    {
        unique_lock<std::mutex> lck(tempo);
        tempoAttuale=tim->getTimeInMin();
        #ifdef DEBUG_MODE
        cout<<"[STRATEGIA -] tempo Attuale"<< tempoAttuale<<" ultima misurazione "<<tempoUltimaMisurazione<<endl;
        #endif

    }
    float EnergiaConsumata=consumo(tempoAttuale);
    #ifdef DEBUG_MODE
    cout<<"[STRATEGIA -] energia consumata"<< EnergiaConsumata<<"\n";
    #endif
    EnergiaEffettivaAccumulata=energiaArrivata(tempoAttuale);
    gap = GAP();
    #ifdef DEBUG_MODE
    cout<<"[STRATEGIA -] energia Accumulata "<< EnergiaEffettivaAccumulata<<" con Gap "<< gap<<"\n";
    #endif
    EnergiaPrevista = previsione(tempoAttuale);
    if(EnergiaPrevista==0){
        #ifdef DEBUG_MODE
        cout<<"[STRATEGIA -] NIENTE LUCE NELLA PROSSIMA MEZZ'ORA\n";
        #endif
    }
    #ifdef DEBUG_MODE
    cout<<"[STRATEGIA -] energiaPrevista"<< EnergiaPrevista<<"\n";
    #endif
    tempoUltimaMisurazione=tim->getTimeInMin();
    int i= configurazione();
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

    batteryStatus = getBatteryCharge();
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
    pthread_setname_np(pthread_self(), "Gestione Host");
    while(true){
        {
            unique_lock<std::mutex> lck(host);
            if(riconfigurare){
                riconf.wait(lck);
            }
        }
            {unique_lock<std::mutex> lck2(cfgfilelock);
            if(off){
                flagOff.wait(lck2);
            }
        }

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
    while(true){
    {
        unique_lock<std::mutex> lck(tempo);
        tim->incrementsMinutes(1);
    }
    slotAttuale=(int)(tim->getMinutes()+tim->getHours()*60)/FREQUENZA;
    //tempo.incrementa(40);
    #ifdef VIRTUALE
    cout << "[T GESTIONE TEMPO] sono passati 5 minuti, siamo al "<<tim->getTimeInMin()<<" minuto\n";
    #endif
    this_thread::sleep_for(chrono::seconds(2));
    //sleep(10);
    }
    return nullptr;
}

/* +++++++++++++++++++++ ROUTINE ++++++++++++++++++++++++++++++++*/

    void routineSpegni(int& interrupt){
        int exitcode = system(comandoDown.c_str());
        interrupt =-1;
    }

    void routineCambioConfigurazione(int &interrupt){
        #ifdef DEBUG_MODE
        cout<<"[MAIN - ROUT Cambio cfg] "<<interrupt<<endl;
        #endif
        configurazioneAttuale=interrupt;
        int exitcode;
        unique_lock<std::mutex> lck1(host);
        unique_lock<std::mutex> lck(cfgfilelock);
        switch (configurazioneAttuale)
        {
        case 0:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 0] "<<interrupt<<endl;
            #endif
            off=true;
            exitcode = system(comandoDown.c_str());
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 0 down] "<<cfgfilename<<endl;
            #endif
            flagOff.notify_all();
            break;
        case 1:
        #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 1] "<<interrupt<<endl;
            #endif
            off=false;
            cfgfilename = "../cfg/fmxcku115r1_1.cfg";
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 1] "<<cfgfilename<<endl;
            #endif
            exitcode = system(comandoUp.c_str());
            flagOff.notify_all();
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
            exitcode = system(comandoUp.c_str());
            flagOff.notify_all();
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
            exitcode = system(comandoUp.c_str());
            flagOff.notify_all();
            break;
        default:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf default] "<<interrupt<<endl;
            #endif
            off=true;
            exitcode = system(comandoDown.c_str());
            flagOff.notify_all();
            break;
        }
        riconfigurare=true;
        riconf.notify_all();

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
    // pthread_setname_np(pthread_self(), "Main Thread");
    //creazione della strategia
    
    inizializzazione();
    int result = pthread_create(&normalThread1, nullptr, gestioneTempo, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
        return 1;
    }
    #ifdef DEBUG_MODE
    cout<<"[MAIN] creo thread che gestisce l'host"<<endl;
    #endif
    result = pthread_create(&normalThread1, nullptr, gestioneHost, nullptr);
    if (result != 0) {
        std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
        return 1;
    }
    EnergiaPrevista=modelloPannello->getProducedPowerByTime(tim->getTimeInMin())*FINESTRAPREDIZIONE;
    #ifdef DEBUG_MODE
    cout<<"[MAIN] Predizione iniziale "<< EnergiaPrevista<<endl;
    #endif
    while(c<100){
        
        //runFpga();
       

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
        }

        
        // //2. thread che gestice la comunicazione con l'host
        // exitcode = system(comand.c_str());
        
        if(flagInterrupt){
            #ifdef DEBUG_MODE
            cout<<"[MAIN] controllo interrupt"<<endl;
            #endif
            checkInterrupt();
        }
        flagInterrupt=false;
        c++;
        sleep(2);
    }

    #ifdef DEBUG_MODE
        cout<<"[MAIN] fine"<<endl;
        #endif
    return 0;
}

