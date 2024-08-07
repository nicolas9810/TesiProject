#include "batteria.h"
#include <iostream>


Batteria::Batteria(int enMax,float e, float kappa, float con,int finestra){
        Emax=enMax;
        E=Emax;
        eta=e;
        k=kappa;
        c=con;
        F=finestra;
        gap=0;
}
Batteria::Batteria(){

}

void  Batteria::setEnergiaMax(float e)
{
     Emax=e;
}
void  Batteria::setEnergia(float e)
{
    if(e>Emax){
        E=Emax;
    }
    else{
        E=e;
    }
}
float Batteria::setEnergiaAccumulata(float e)
{
    Ea=e;
    return Ea;
}
void  Batteria::setEnergiaConsumata(float e )
{
    Ec=e;
}

void  Batteria::setEnergiaPrevista(float e )
{
    Ep=e;
}
void  Batteria::setPotenzaIn( float p)
{
    Pin=p;
}
void  Batteria::setPotenzaOut( float p)
{
    Pout=p;
}
void  Batteria::setEfficienza( float e)
{
    eta=e;
}
void  Batteria::setCoefPerdite( float p)
{
    k=p;
}
float Batteria:: getEnergiaMax(){
    return Emax;
}
float Batteria:: getEnergia(){
    return E;
}
float Batteria:: getEnergiaAccumulata(){
    return Ea;
}
float Batteria:: getEnergiaConsumata(){
    return Ec;
}
float Batteria:: getPotenzaIn(){
    return Pin;
}
float Batteria:: getPotenzaOut()
{
    return Pout;
}
float Batteria:: getEfficienza()
{
    return eta;
}
float Batteria:: getCoefPerdite()
{
    return k;
}

int Batteria::SoC(){
        return (E/Emax)*100;
}

float Batteria::charge(float potenza,int tempo)
{
    Pin=potenza;
    int tmp = Pin*(eta-k)*tempo;
    if(tmp+E>Emax){
        E=Emax;
    }
    else{
        E+=tmp;
    }
    return E;
}

float Batteria::discharge(float potenza,int tempo)
{
    Pout=potenza;
    int tmp=Pout*tempo;
    E-=tmp;
    if(E<0){
        E=0;
    }
    return E;
}
float Batteria::chargeDischarge(){
    std::cout<<"[BATTERIA - pre] Energia ="<< E<<std::endl;
    E+=Ea*(eta-k);
    E-=Ec;
    if(E>Emax){
        E=Emax;
    }else if(E<0){
        E=0;
    }
    std::cout<<"[BATTERIA - post] Energia ="<< E<<std::endl;

    return E;
}

float Batteria::energiaAccumulata(int p,int t)
{
    Ea=p*t/60;
    return Ea;
}

float Batteria::energiaConsumata(int p,int t)
{
    Ec=p*t/60;
    return Ec;
}
float Batteria::energiaPrevista(int p,int t)
{
    Ep=p*t/60;
    return Ep;
}

int Batteria::findConfiguration(float *potenze,int f)
{
    float soc=SoC();
    if(soc<90){
        return 0;
    }
    bool flag=false;
    int i=3;
    while(i>0 && !flag){
           
        float consumo=potenze[i]*2;
        float proiezione=Ep*6-consumo+gap;
        #ifdef DEBUG_MODE
        std::cout<<"[Cerco conf -] conf = "<< i <<", previsione"<<proiezione<<std::endl;
        #endif
        if((proiezione/Emax)*100>c){
            flag=true;
            break;
        }
        i--;
    }
    return i;
}
