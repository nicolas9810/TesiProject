#include "batteria.h"

Batteria::Batteria(int enMax,float e, float kappa, float con,int finestra){
        Emax=enMax;
        eta=e;
        k=kappa;
        c=con;
        F=finestra;
        gap=0;
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
void  Batteria::setEnergiaAccumulata(float e)
{
    Ea=e;
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

float Batteria::SoC(){
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

float Batteria::energiaAccumulata(int p,int t)
{
    Ea=p*t;
    return Ea;
}

float Batteria::energiaConsumata(int p,int t)
{
    Ec=p*t;
    return Ec;;
}
float Batteria::energiaPrevista(int p,int t)
{
    Ep=p*t;
    return Ep;
}

int Batteria::findConfiguration(int *potenze)
{
    float soc=SoC();
    if(SoC()<c){
        return 0;
    }
    bool flag=false;
    int i=3;
    while(i>0 || flag){
        float consumo=potenze[i]*F;
        float proiezione=Ep-consumo+gap+E;
        if(proiezione>c){
            flag=true;
            break;
        }
        i--;
    }
    return i;
}
