#include <iostream>
#include "strategiaPredizioneSimul.h"
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

StrategiaPredizioneSimul::StrategiaPredizioneSimul(): Strategia::Strategia(nullptr){
    model=GreenPlantModel("./csv/2019.csv");
}
StrategiaPredizioneSimul::StrategiaPredizioneSimul(epever *dev): Strategia::Strategia(dev){
    attivo=true;
    giorno=0;
    model=GreenPlantModel("./csv/2019.csv");
    std::thread previsioni([this]{gestionePrevisioni();});
}
StrategiaPredizioneSimul& StrategiaPredizioneSimul::operator=(const StrategiaPredizioneSimul& other){
     if (this != &other) {
            // Chiamata all'operatore di assegnazione della classe base
            Strategia::operator=(other);

            // Implementazione aggiuntiva dell'operatore di assegnazione della classe derivata
            // Se necessario...
        }
    return *this;
}

int StrategiaPredizioneSimul::strategia(float batteryCharge){
    // float batteryCharge=getBatteryCharge();
    float previsioneEnergia = previsioneEnergiaDisponibile(PREDWINDOW);
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
int StrategiaPredizioneSimul::getBestConfig(float predizione){
    int delta;
    for(int i=MAXPOWER;i>=OFF;i--){
        delta=predizione-arrayConsumi[i].consumo;
        //se trovo una configurazione che rientra
        if(delta>0){
            return i;
        }
    }
    //se non trovo niente, significa che non può caricarsi in questo momento
    return -1;

}

float StrategiaPredizioneSimul::misuraPotenza(){
    time_t now = time(0); // get current date and time  
    tm* ltm = localtime(&now);  
    int hour= ltm->tm_hour;
    int min= ltm->tm_min;
    min=min+hour*60+giorno*24*60;
    return model.getProducedPowerByTime(min);
}


void StrategiaPredizioneSimul::gestionePrevisioni(){
    CodaCircolare codaMisurazioni=CodaCircolare(5);
    inizializzaMatriceTest(model);

    int ultimoSlot=0;
    while(true){
 
        /* 
        Attivo il trhead solo se questa strategia viene utilizza
        Per fare ciò faccio attendere il thread su una condition variable
        */
        unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this](){ return attivo; }); // Il thread rimane in attesa finché isReady diventa true
        #ifdef DEBUG_MODE
        cout << "Il thread è stato sbloccato!\n";
        #endif
        /*
        se inizia un nuovo giorno devo fare uno shift verso sinistra di tutta la matrice e rifare le previsioni per il giorno
        */
        if(giornoNuovo){
            #ifdef DEBUG_MODE
            cout<<"È iniziato un nuovo giorno, calcolo le nuove previsioni"<<endl;
            #endif
            giorno++;
            nuovoGiorno();
            previsioneDelGiorno(0);
            giornoNuovo=false;
            }
        /*
        altrimenti faccio un minimo di 5 misurazioni ogni (FREQUENZA*60/15) secondi e calcolo la media che inserisco nella matrice energetica
        */
        else{
            float potenzaInIngresso=misuraPotenza();
            //effettuo una misurazione ogni FREQUENZA/10 min ed ogni FREQUENZA/2 prendo la media delle ultime 5 misruazioni
            // e la inseirsco nella matrice delle misurazioni per aggiornare le previsioni
            for(int i=0;i<5;i++){
                #ifdef DEBUG_MODE
                cout<<"Misura potenza in ingresso: "<<potenzaInIngresso<<endl;
                #endif
                codaMisurazioni.addElement(potenzaInIngresso);
                this_thread::sleep_for(chrono::minutes(FREQUENZA/10));
            }
            float mediaMisurazioni=codaMisurazioni.getMedia();
            ultimoSlot=setEnergia(mediaMisurazioni);
            #ifdef DEBUG_MODE
            cout<<"slot del giorno: "<<ultimoSlot<<" = "<<media<<endl;
            #endif

            if(ultimoSlot==NN/2 && !giornoUno){
                previsioneDelGiorno(ultimoSlot);
                #ifdef DEBUG_MODE
                cout<<"È mezzogiorno, riaggiorno le previsioni"<<endl;
                #endif
            }
            if(ultimoSlot>=NN-1){
                #ifdef DEBUG_MODE
                cout<<"È l'ultimo slot del giorno, pronto a passare al prossimogiorno"<<endl;
                #endif
               giornoNuovo=true;
               if(giornoUno)giornoUno=false;
            }
            
        }
    }    
}

void StrategiaPredizioneSimul::attiva(){
    {
        lock_guard<std::mutex> lock(mtx);
        attivo = true;
    }
    cv.notify_one();
}

void StrategiaPredizioneSimul::disattiva(){
        lock_guard<std::mutex> lock(mtx);
        attivo = false;
}