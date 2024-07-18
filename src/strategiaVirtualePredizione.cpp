#include <iostream>
#include "strategiaVirtualePredizione.h"
#include "defines.h"
#include "strategia.h"
#include "previsione.h"
#include <thread>
#include <chrono>


using namespace std;

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

StrategiaVirtualePredizione::StrategiaVirtualePredizione(): Strategia::Strategia(nullptr){}
StrategiaVirtualePredizione::StrategiaVirtualePredizione(epever *dev,Tempo *t): Strategia::Strategia(dev){
    
    attivo=true;
    setTempo(t);
    // modelloPannello=new GreenPlantModel("/home/nicolas/Codice/TesiProject/csv/2019.csv");
    // for(int i=0;i<DAYS;i++){
    //         // int temp=i*24*60;
    //     for(int j=0;j<NN;j++){
    //         // temp+=60;
    //         time->incrementsMinutes(60);
    //         setEner(i,j,modelloPannello->getProducedPowerByTime(time->getTimeInMin()));
    //     }
    // }
    //std::thread previsioni([this]{gestionePrevisioni();});
}


StrategiaVirtualePredizione& StrategiaVirtualePredizione::operator=(const StrategiaVirtualePredizione& other){
     if (this != &other) {
            //Chiamata all'operatore di assegnazione della classe base
            Strategia::operator=(other);

            // Implementazione aggiuntiva dell'operatore di assegnazione della classe derivata
            // Se necessario...
        }
    return *this;
}

int StrategiaVirtualePredizione::strategia(float batteryCharge,int conf){
    // float batteryCharge=getBatteryCharge()
    int min=getTempo()->getMinutes()+getTempo()->getHours()*60;
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
int StrategiaVirtualePredizione::getBestConfig(float predizione){
    int delta;
    int actual=modelloPannello->getProducedPowerByTime(time->getTimeInMin());
    for(int i=MAXPOWER;i>=OFF;i--){
        delta=((predizione+actual)/2 )-arrayConsumi[i].consumo; 
        //delta=predizione-arrayConsumi[i].consumo;
        #ifdef VIRTUALE
        std::cout << "[T STRATEGIA VIRTUALE PRED] DELTA = "<<delta<<"\n";
        #endif
        //se trovo una configurazione che rientra
        if(delta>0){
            return i;
        }
    }
    //se non trovo niente, significa che non può caricarsi in questo momento
    return -1;

}

float StrategiaVirtualePredizione::misuraPotenza(epever* device){
    return device->getArrayPower();
}


void StrategiaVirtualePredizione::gestionePrevisioni(){
    gestionePrevisioniVirtuale(time,modelloPannello);
    // pthread_setname_np(pthread_self(),"Strategia");
    // CodaCircolare codaMisurazioni=CodaCircolare(5);
    //  #ifdef DEBUG_MODE
    // cout<<"[GESTIONE PREVISIONI] inizializzo matrice"<<endl;
    // #endif
    // inizializzaMatrice();
    // int ultimoSlot=0;
    // int giorno=-1;
    // bool flag = false;
    // while(true){

    //     /*
    //     Attivo il trhead solo se questa strategia viene utilizza
    //     Per fare ciò faccio attendere il thread su una condition variable
    //     */
        
    //     /*unique_lock<std::mutex> lock(mtx);
    //     cv.wait(lock, [this](){ return attivo; }); // Il thread rimane in attesa finché isReady diventa true
    //     #ifdef DEBUG_MODE
    //     cout << "Il thread è stato sbloccato!\n";
    //     #endif
    //     */
    //     /*
    //     se inizia un nuovo giorno devo fare uno shift verso sinistra di tutta la matrice e rifare le previsioni per il giorno
    //     */
    //     if(time->getDay()!=giorno){
    //         #ifdef DEBUG_MODE
    //         cout<<"È iniziato un nuovo giorno, calcolo le nuove previsioni"<<endl;
    //         #endif
    //         nuovoGiorno();
    //         //    previsioneDelGiorno(0);
    //             flag = false;
    //             giorno=time->getDay();
    //         //    giornoNuovo=false;

    //     }
    //     /*
    //     altrimenti faccio un minimo di 5 misurazioni ogni (FREQUENZA*60/15) secondi e calcolo la media che inserisco nella matrice energetica
    //     */
    //     else{

    //         float potenzaInIngresso=misuraPotenza(getDevice());
    //         //effettuo una misurazione ogni FREQUENZA/10 min ed ogni FREQUENZA/2 prendo la media delle ultime 5 misruazioni
    //         // e la inseirsco nella matrice delle misurazioni per aggiornare le previsioni
    //         int min = time->getTimeInMin();
    //         int minDelGiorno=time->getMinutes();
    //         int oraDelGiorno = time->getHours();

    //         // aspetto di ottenere il primo dato della giornata prima di effettuare una predizione

    //         for(int i=0;i<5;i++){
    //             #ifdef DEBUG_MODE
    //             cout<<"Misura potenza in ingresso: "<<potenzaInIngresso<<endl;
    //             #endif
    //             codaMisurazioni.addElement(potenzaInIngresso);
    //             this_thread::sleep_for(chrono::seconds(FREQUENZA/10));
    //         }
    //         float mediaMisurazioni=codaMisurazioni.getMedia();
    //         double misurazioni=modelloPannello->getProducedPowerByTime(min);
    //         setEnergia(misurazioni,minDelGiorno+oraDelGiorno*60);

    //         //ultimoSlot=setEnergia(mediaMisurazioni,min);
    //         #ifdef DEBUG_MODE
    //         cout<<"slot del giorno: "<<ultimoSlot<<" = "<<media<<endl;
    //         #endif
    //         if(misurazioni>0 && !flag){
    //             flag = true;
    //             int slot=(minDelGiorno+oraDelGiorno*60)/60;
    //             previsioneDelGiorno(slot,misurazioni);
    //             printPrevisioni();
    //         }
    //     }
    // }
}

void StrategiaVirtualePredizione::attiva(){
    {
        lock_guard<std::mutex> lock(mtx);
        attivo = true;
    }
    cv.notify_one();
}

void StrategiaVirtualePredizione::disattiva(){
        lock_guard<std::mutex> lock(mtx);
        attivo = false;
}

// int StrategiaVirtualePredizione::getTimeInMin(){
//     time_t now = time(0); // get current date and time
//     tm* ltm = localtime(&now);
//     int hour= ltm->tm_hour;
//     int min= ltm->tm_min;
//     min=min+hour*60;
//     return min;
// }
