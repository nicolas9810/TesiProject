#include <iostream>
#include "strategiaGreedy.h"
#include "defines.h"
#include "strategia.h"

#define DEBUG_MODE

bool DEBUG = true;



using namespace std;


StrategiaGreedy::StrategiaGreedy(epever *dev): Strategia::Strategia(dev){}
StrategiaGreedy::StrategiaGreedy():Strategia::Strategia(nullptr){}
int StrategiaGreedy::strategia(float batteryCharge,int conf){
    bool inCharge=Strategia::getBatteryStatus();
    // int batteryCharge=Strategia::getBatteryCharge();
    if(batteryCharge <30){
        #ifdef DEBUG_MODE
        cout<<"Batteria minore del 30\%: "<<batteryCharge<<endl;
        #endif
        if(!inCharge){
            #ifdef DEBUG_MODE
            cout<<"Batteria in Scarico: il sistema viene spento"<<endl;
            #endif
            return OFF;
        }else{
            #ifdef DEBUG_MODE
            cout<<"Batteria in CARICA: il sistema viene messo a minima potenza"<<endl;
            #endif
            return MINPOWER;
        }
    }
    else if(batteryCharge>= 30 && batteryCharge <50){
        #ifdef DEBUG_MODE
        cout<<"Batteria tra il 30\% e il 50\%: "<<batteryCharge<<endl;
         #endif
        if(inCharge){
            #ifdef DEBUG_MODE
            cout<<"Batteria in CARICA: il sistema viene messo a media potenza"<<endl;
            #endif
            return MEDPOWER;
        }else{
            #ifdef DEBUG_MODE
            cout<<"Batteria in SCARICO: il sistema viene messo a minima potenza"<<endl;
            #endif
            return MINPOWER;
        } 
    }
    else if(batteryCharge>= 50 && batteryCharge<70){
        #ifdef DEBUG_MODE
        cout<<"Batteria tra il 50\% e il 70\%: "<<batteryCharge<<endl;
        #endif
        if(inCharge){
            #ifdef DEBUG_MODE
            cout<<"Batteria in CARICA: il sistema viene messo a massima potenza"<<endl;
            #endif
            return MAXPOWER;
        }else{
            #ifdef DEBUG_MODE
            cout<<"Batteria in SCARICO: il sistema viene messo a minima potenza"<<endl;
            #endif
            return MEDPOWER;
        }
    }
    else{
        #ifdef DEBUG_MODE
        cout<<"Batteria oltre il 70\%: "<<batteryCharge<<endl;
        cout<<"il sistema viene messo a massima potenza: "<<batteryCharge<<endl;
        #endif
        return MAXPOWER;
    }
}
void StrategiaGreedy::gestionePrevisioni(){
    return;
}
