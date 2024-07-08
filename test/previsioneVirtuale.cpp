#include <ctime> 
#include "defines.h"
#include <iostream>
#include "previsioneVirtuale.h"

// idealmente sono da aggiustare dinamenicamente
const int alfa = 0.7;
const int kappa = 4;

int oggi= DAYS-1;
int ieri = DAYS-2;

int matrice_energetica [DAYS][NN];
// int matrice_previsione_energetica [DAYS][NN];
float medie_energetiche [NN];

float previsioneEnergiaDisponibile(int n,Tempo *time){
   

    int hour= time->getHours();
    int min= time->getMinutes();
    min=min+hour*60;
    int slot=min/FREQUENZA;
    float result;
    int c=0;
    for(int i=slot;i<slot+n && i<NN;i++){
        c++;
        result+=matrice_energetica[DAYS][i];
    } 
    return result/c;
}


int setEnergia(float e,Tempo *time){
    int hour= time->getHours();
    int min= time->getMinutes();
    min=min+hour*60;
    min=min+hour*60;
    int slot=min/FREQUENZA;
    matrice_energetica[oggi][slot]=e;
    return slot;
}

void inizializzaMatrice(){
    for(int i=0;i<DAYS;i++){
        for(int j=0;j<NN;j++){
            matrice_energetica[i][j]=-1;
            //matrice_previsione_energetica [i][j]=-1;
        }
    }
}

void inizializzaMatriceTest(GreenPlantModel model){
    for(int i=0;i<DAYS;i++){
        for(int j=0;j<NN;j++){
            int minuti=i*60+j*FREQUENZA;
            matrice_energetica[i][j]=model.getProducedPowerByTime(minuti);
            //matrice_previsione_energetica [i][j]=-1;
        }
    }
}


void media(){
    float somma;
    int count;
    for(int i=0;i<NN;i++){
        somma=0;
        count=0;
        for(int j=0;j<DAYS;j++){
            int tmp=matrice_energetica[j][i];
            if(tmp!=-1){
                somma+=tmp;
                count++;
            }
        }
        if(count!=0){
            medie_energetiche[i]=somma/count;
        }else 
            medie_energetiche[i]=1;
    }
}

int setEnergia(int n,int e){
    if(n>NN){
        return -1;
    }
    else 
        matrice_energetica[oggi][n]=e;
    return 0;
}
void nuovoGiorno(){
    for(int i=0;i<DAYS-1;i++){
        for(int j=0;j<NN;j++){
            matrice_energetica[i][j]=matrice_energetica[i+1][j];   
        }
    }
    media();
}


float prodottoScalare(float v1[],float v2[]){
        float somma=0;
        for(int k=0;k<kappa;k++){
            somma+=v1[k]*v2[k];
        }
    return somma;
}


float gapk(int i){
    float gap=0;
    if(i<kappa){
        float v [i];
        float p [i];
        float sommatoriaP=0;

        for(int k=0;k<i;k++){
            v[k]=(matrice_energetica[oggi][k])/medie_energetiche[k];
            p[k]=(k+1)/i;
            sommatoriaP+=p[k];
        }
        gap= prodottoScalare(v,p)/sommatoriaP;
    }

    else{
        float v [kappa];
        float p [kappa];
        float sommatoriaP=0;

        for(int k=0;k<kappa;k++){
            v[k]=(matrice_energetica[oggi][i-kappa+k])/medie_energetiche[i-kappa+k];
            p[k]=(k+1)/kappa;
            sommatoriaP+=p[k];
        }
        gap= prodottoScalare(v,p)/sommatoriaP;
    }
    return gap;
}



void previsioneDelGiorno(int c){
    for(int i = c;i< NN;i++){
        float mediaSlot=medie_energetiche[i];
        if(i==0){
            matrice_energetica[oggi][i]= medie_energetiche[i];
        }
        matrice_energetica[oggi][i]= alfa * matrice_energetica[oggi][i-1] * (1-alfa)*gapk(i+1)* medie_energetiche[i];
    }
}

void printMatriceEnergetica(){
    for (int i = 0; i < DAYS; ++i) {
        for (int j = 0; j < NN; ++j) {
            std::cout << matrice_energetica[i][j] << " ";
        }
        std::cout << std::endl;
    }
}












