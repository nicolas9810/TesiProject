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

#include <fstream>
#include <filesystem>  // Per creare la cartella Log

#include "Modello.h"
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

int tempoUltimaMisurazione;
int slotAttuale;
float EnergiaPrevista;
float EnergiaEffettivaAccumulata;
float gap;
// GreenPlantModel *modelloPannello;
Modello *modelloPannello;

Batteria *batteria;

float arraytempo []= {  0, 0, 0, 0, 0, 0, 7, 163, 110, 468, 595, 471, 386, 635, 523, 89, 321, 55, 7, 0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0, 0, 7, 22, 44, 65, 372, 390, 327, 529, 789, 620, 415, 195, 21, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 106, 331, 561, 763, 907, 996, 1013, 960, 836, 662, 446, 214, 28, 0, 0, 0, 0, 0,
                    } ;

string cfgfilename = "../cfg/fmxcku115r1_1.cfg";
//string comandoUp ="profpga_run "+cfgfilename+" --up";
// string comandoDown ="profpga_run "+cfgfilename+" --down";

chrono::time_point<std::chrono::high_resolution_clock> inizio;
chrono::time_point<std::chrono::high_resolution_clock> fine;

chrono::time_point<std::chrono::high_resolution_clock> iniziocfg;
chrono::time_point<std::chrono::high_resolution_clock> finecfg;


void startTimer(chrono::time_point<std::chrono::high_resolution_clock> &inizio) {
    inizio = std::chrono::high_resolution_clock::now();
}

double getElapsedTime(chrono::time_point<std::chrono::high_resolution_clock> &fine) {
    fine = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = fine - inizio;
    return elapsed.count();
}


int tempiConfig [4];



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
    //device = new epever("/dev/ttyXRUSB0");
    for(int i=0;i<INTERRUPTS;i++){
        interrupt[i]=-1;
    }
    // modelloPannello=new GreenPlantModel("../csv/2018.csv");

    modelloPannello=new Modello();
    modelloPannello->setPrevisioni(arraytempo,72);

    // pthread_setname_np(pthread_self(), "Main Thread");
    //creazione della strategia
    tim=new Tempo(0,0,0,0);
    giornoAttuale=0;
    configurazioneAttuale=0;
    tempoUltimaMisurazione=tim->getTimeInMin();
    //Batteria ( Energia Max, efficienza, coef di peridite, limite di scaricamento, finestra di predizione)
    batteria=new Batteria(ENMAX,EFFICIENCY,LOSS,MINIMUMCHARGE,FINESTRAPREDIZIONE);
    batteria->setEnergia(600);
   

}


void writeLog(const std::string& filename, const std::string& message) {
    std::string filePath = "../Log/" + filename;
    std::ofstream logFile;
    logFile.open(filePath, std::ios_base::app); // Apri il file in modalità append
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.close();
    } else {
        std::cerr << "Unable to open log file" << std::endl;
    }
}


float getBatteryCharge(){
    return 90;//device->getBatterySOC();
}
float misuraPotenza(){
    return 0;//device->getArrayPower();
}

float previsione(int t){
    
    //ritornare energia nella finestra di tempo successiva con una percentaule di errore del 20/15%*/
    float delta_t=1.0/60.0;
    float energiaInArrivo=0;
    for(int i= t;i< t+FINESTRA;i++){
        energiaInArrivo+= modelloPannello->getProducedPowerByTime(i)*delta_t;
        if(modelloPannello->getProducedPowerByTime(i)*delta_t>0){
        }
    }
    float erroreRandom= static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    
    // Scala il valore nel range -0.20 a 0.20
    float en= energiaInArrivo*((erroreRandom * 0.40f) - 0.20f);
    #ifdef DEBUG_MODE
    cout<<"[previsione -] energia prevista "<< (energiaInArrivo+en) <<" energia vera "<< energiaInArrivo<<"\n";
    #endif
    batteria->setEnergiaPrevista((energiaInArrivo+en));
    return (energiaInArrivo+en);
}

float GAP(){
    float energia =EnergiaEffettivaAccumulata- EnergiaPrevista;
    #ifdef DEBUG_MODE
    cout<<"[ GAP ] EnergiaPrevista "<< EnergiaPrevista <<" - EnergiaEffettivaAccumulata" << EnergiaEffettivaAccumulata<<"\n";
    #endif
    return energia;
}

float consumi[]={40,89,120,150};


int findts(){
    int t_s=0;
    {
    //unique_lock<std::mutex> lck(tempo);
    t_s=tim->getTimeInMin()+FINESTRA;
    }
    int incr=0;
    bool flag=false;
    while(incr<1440 && !flag){
        int P_in=modelloPannello->getProducedPowerByTime(t_s+incr);
        if(P_in>consumi[0]){
            flag=true;
            t_s+=incr;
        }
        incr++;
   }
   #ifdef DEBUG_MODE
   cout<<" [FIND TS ] t_s arriva fra "<< incr<< " min"<<endl;
   #endif
   return t_s;

}

int configurazione(){
    int t_n=0;
    {
    //unique_lock<std::mutex> lck(tempo);
    t_n=tim->getTimeInMin();
    }
    float E_p= previsione(t_n);
    float E_batteria=batteria->getEnergia();
    bool flag=false;
    #ifdef DEBUG_MODE
    cout<<"[CONFIG] energia prevista ="<< E_p<<endl;
    cout<<"[CONFIG] energia batteria ="<< E_batteria<<endl;
    #endif
    // while(i>0 && !flag){
    for(int i=3;i>0;i--){
        float E_consumo=consumi[i]*FINESTRA/60;
        bool cond1= E_p-E_consumo+E_batteria>0;
        // #ifdef DEBUG_MODE
        // cout<<"[CONFIG] En consuma"<< i <<" = " << E_consumo<<endl;
        // #endif
        if(cond1){
        #ifdef DEBUG_MODE
        cout<<"[CONFIG] cond1 ="<< E_p-E_consumo+E_batteria <<" SODDISFATTA "<<endl;
        #endif
            bool cond2= E_p > consumi[0]*FINESTRA/60;
            if(cond2){
                #ifdef DEBUG_MODE
                cout<<"[CONFIG] scelta cfg "<<i<<endl;
                #endif
                return i;
            }
            else{
                int t_s = findts();
                float E_t=E_batteria;
                bool flag2 = true;
                double delta_t=1.0/60.0;
                for(int t=t_n;t<t_s;t++){
                    float E_in = (modelloPannello->getProducedPowerByTime(t))*delta_t;
                    float E_out;
                    if(t<t_n+FINESTRA){
                        E_out = consumi[i]*delta_t;
                    }
                    else{
                        E_out = consumi[0]*delta_t;

                    }
                    E_t= E_t + E_in - E_out;
                    bool cond3= E_t>0;
                    if(!cond3){
                        flag2 = false;
                        #ifdef DEBUG_MODE
                        cout<<"[CONFIG] cond3 at tempo t < t_n+f "<< t <<" NON SODDISFATTA "<<endl;
                        #endif
                        break;
                    }
                }
                if(flag2){
                     #ifdef DEBUG_MODE
                    cout<<"[CONFIG] scelta cfg "<<i<<endl;
                    #endif
                    return i;
                }
            }
        }
    }
    #ifdef DEBUG_MODE
    cout<<"[CONFIG] scelta cfg "<<0<<endl;
    #endif
    return 0;
}
float consumo(int t){
    return batteria->energiaConsumata(consumi[configurazioneAttuale],(t-tempoUltimaMisurazione));
}

float energiaArrivata(int t){
    
    float potenzaInput=0;
    double delta_t=1.0/60.0;
    for(int i= tempoUltimaMisurazione;i< t;i++){
        potenzaInput+= modelloPannello->getProducedPowerByTime(i);
    }
    std::cout<<"[ENERGIA ARRIVATA ="<< potenzaInput*delta_t<<" che diventa "<<(potenzaInput*delta_t)*0.8<<std::endl;

    return batteria->setEnergiaAccumulata(potenzaInput*delta_t);
}


int strategia(){
    float soc=batteria->SoC();
    if(soc==0){
        cout<<"BATTERIA ALLO 0%\n";
    }
    int tempoAttuale;
    {
    //unique_lock<std::mutex> lck(tempo);
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
    EnergiaPrevista = previsione(tempoAttuale);
    if(EnergiaPrevista==0){
        #ifdef DEBUG_MODE
        cout<<"[STRATEGIA -] NIENTE LUCE NELLA PROSSIMA FINESTRA\n";
        #endif
    }
    #ifdef DEBUG_MODE
    cout<<"[STRATEGIA -] energiaPrevista "<< EnergiaPrevista<<"\n";
    #endif
    }
    
    int cfg= configurazione();
    tempoUltimaMisurazione=tim->getTimeInMin();
    
    #ifdef DEBUG_MODE
    cout<<"[STRATEGIA -] scelta configurazione "<< cfg <<"\n";
    #endif

    return cfg;
    
}
void gestioneStrategia(){
    #ifdef DEBUG_MODE
    cout<<"[T GESTIONE STRATEGIA] inzio a monitorare l'energia"<<endl;
    #endif    
    int configurazione;
    float batteryStatus;
    //currentStrategy->setConsumi(configurazione);
    configurazione=strategia();
    if (configurazione!=configurazioneAttuale){ 
        startTimer(inizio);  
        #ifdef DEBUG_MODE
        cout<<"[GESTIONE STRATEGIA ] Nuova configurazione scelta, attuale = "<<configurazioneAttuale<<", nuova= "<<configurazione<<endl;
        #endif
        
        interrupt[INTCONFIG]=configurazione;
        flagInterrupt=true;             
    }  
    else{
        #ifdef DEBUG_MODE
        cout<<"[ GESTIONE STRATEGIA ] configurazione è adatta = "<<configurazioneAttuale<<endl;
        #endif
    }
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
        //unique_lock<std::mutex> lck(tempo);
        tim->incrementsMinutes(1);
    }

    int minuti=tim->getMinutes();
    int ore=tim->getHours();
    int giorno=tim->getDay();
    if(giorno!=giornoAttuale){
        giornoAttuale=giorno;
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
        double finetempo = getElapsedTime(finecfg);
        tempiConfig[configurazioneAttuale]+=finetempo;
        configurazioneAttuale=interrupt;
        int exitcode;
        string comandoDown;
        string comandoUp;
        string logFile;
        string msg;

        switch (configurazioneAttuale)
        {
        case 0:
            #ifdef DEBUG_MODE
            cout<<"[MAIN - ROUT conf 0] "<<interrupt<<endl;
            #endif
            off=true;
            logFile="cambio_configurazione_0.txt";
            msg="[ LOG ] CFG 0 - spengo il sistema ";
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
            logFile="cambio_configurazione_1.txt";
             msg="[ LOG ]CFG 1 - scelta configurazione 1 ";
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
             logFile="cambio_configurazione_2.txt";
             msg="[ LOG ] CFG 2 - scelta configurazione 2 ";
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
             logFile="cambio_configurazione_3.txt";
             msg="[ LOG ] CFG 3 - scelta configurazione 3 ";
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

        double elapsed=getElapsedTime(fine);
        writeLog(logFile,msg);
        string tempo = "[ TEMPO ]" + to_string(elapsed);
        writeLog(logFile,tempo);

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
    // int result = pthread_create(&normalThread1, nullptr, gestioneTempo, nullptr);
    // if (result != 0) {
    //     std::cerr << "Errore nella creazione del thread a priorità normale" << std::endl;
    //     return 1;
    // }
    #ifdef DEBUG_MODE
    cout<<"[MAIN] creo thread che gestisce l'host"<<endl;
    #endif

    startTimer(iniziocfg);

    EnergiaPrevista=modelloPannello->getProducedPowerByTime(tim->getTimeInMin())*FINESTRAPREDIZIONE/60;
    #ifdef DEBUG_MODE
    cout<<"[MAIN] Predizione iniziale "<< EnergiaPrevista<<endl;
    #endif
    bool flag=false;
    while(true){
        int tempoAttuale;
        {
            //unique_lock<std::mutex> lck(tempo);
            tempoAttuale=tim->getTimeInMin();
        }
        if(tempoAttuale>1980){
            break;
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

     std::string multiLinea = "[CFG 0 tempo : " +to_string(tempiConfig[0]) +"\n" \
                            "[CFG 1 tempo : " +to_string(tempiConfig[1]) + "\n"\
                            "[CFG 2 tempo : " +to_string(tempiConfig[2]) + "\n"\
                            "[CFG 3 tempo : " +to_string(tempiConfig[3]) + "\n";

    writeLog("tempi.cfg",multiLinea);

    #ifdef DEBUG_MODE
        cout<<"[MAIN] fine"<<endl;
        #endif
    return 0;
}

