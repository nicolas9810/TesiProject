#include <iostream>
#include "defines.h"
#include "strategia.h"

using namespace std;
Strategia::Strategia(epever* d){
    device=d;
}
bool Strategia::getBatteryStatus(){
    float flag= device->getBatteryCurrentStatus();
    if(flag>0){
        return CHARGING;
    }
    else{
        return DISCHARGING;
    }
}

void Strategia::setConsumi(int c){

    arrayConsumi[c].consumo=getDevice()->getOutputPower(c);

}

epever* Strategia::getDevice(){
    return device;
}

void Strategia::setTempo(Tempo *t){
    tempo=t;
}

Tempo* Strategia::getTempo(){
    return tempo;
} 
