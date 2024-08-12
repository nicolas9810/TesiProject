#include "Modello.h"

Modello::Modello(){}
void Modello::setPrevisioni(float *p,int d){
    previsioni=p;
    dim=d;
}

double Modello::getProducedPowerByTime(double t){
    int index = t/60;
    if(index>dim)return 0;
    return previsioni[index];
}