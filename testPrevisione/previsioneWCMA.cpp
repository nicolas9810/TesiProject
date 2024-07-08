#include <ctime> 
#include "defines.h"
#include <iostream>
#include "previsione.h"


// idealmente sono da aggiustare dinamenicamente
const double alfa = 0.6;
const int kappa = 4;

int oggi= DAYS-1;
int ieri = DAYS-2;

double matrice_energetica [DAYS][NN];
// int matrice_previsione_energetica [DAYS][NN];
double medie_energetiche [NN];

double previsioneEnergiaDisponibile(int n,int min){
    // time_t now = time(0); // get current date and time  
    // tm* ltm = localtime(&now);  
    // int hour= ltm->tm_hour;
    // int min= ltm->tm_min;
    // min=min+hour*60;
    int slot=min/FREQUENZA;
    double result;
    int c=0;
    for(int i=slot;i<slot+n && i<NN;i++){
        c++;
        result+=matrice_energetica[DAYS][i];
    } 
    return result/c;
}


int setEnergia(double e,int min){
    // time_t now = time(0); // get current date and time  
    // tm* ltm = localtime(&now);  
    // int hour= ltm->tm_hour;
    // int min= ltm->tm_min;
    // min=min+hour*60;
    int slot=min/60;
    std::cout<< "slot numero "<<slot<<std::endl;
    matrice_energetica[oggi][slot];
    return slot;
}

void setEner(int i,int j,double e){
    matrice_energetica[i][j]=e;
}


void inizializzaMatrice(){
    for(int i=0;i<DAYS;i++){
        for(int j=0;j<NN;j++){
            matrice_energetica[i][j]=1;
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
    double somma;
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


double prodottoScalare(double v1[],double v2[],int j){
        double somma=0;
        for(int k=0;k<j;k++){
            somma+=v1[k]*v2[k];
        }
    return somma;
}


double gapk(int i){
    double gap=0;
    // if(i<kappa){
    //     double v [i];
    //     double p [i];
    //     double sommatoriaP=0;

    //     for(int k=0;k<i;k++){
    //         if(medie_energetiche[k]==0){
    //             v[k]=0;
    //         }else{
    //             v[k]=(matrice_energetica[oggi][k])/medie_energetiche[k];
    //         }
    //         p[k]=static_cast<double>(k+1)/i;
    //         sommatoriaP+=p[k];
    //     }
    //     gap= prodottoScalare(v,p,i)/sommatoriaP;
    // }

    //else
    {
        double v [kappa];
        double p [kappa];
        double sommatoriaP=0;

        for(int k=1;k<=kappa;k++){
            int index=(i-kappa+k)%NN;
            std::cout<<"indice "<< (i-kappa+k)%NN; 

            if(medie_energetiche[index]==0){
                v[k]=0;
                std::cout<<" v "<< k << "= "<<v[k]<< " ,"; 

            }else{
                v[k]=(matrice_energetica[oggi][index])/medie_energetiche[index];
                std::cout<<"v "<< k << "= "<<v[k]<< " ,"; 
            }
            p[k]=static_cast<float>(k)/kappa;
            sommatoriaP+=p[k];
        }
        std::cout<<std::endl;
        gap= prodottoScalare(v,p,kappa)/sommatoriaP;
    }
    return gap;
}



void previsioneDelGiorno(int c){
    for(int i = c;i< NN;i++){
        double mediaSlot=medie_energetiche[i];
        if(i==0){
            matrice_energetica[oggi][i]= mediaSlot;
        }
        else{
            matrice_energetica[oggi][i]= alfa * mediaSlot + (1-alfa)*gapk(i+1)*matrice_energetica[oggi][i-1];
        }
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

void printPrevisioni(){
    for (int i = 0; i < NN; ++i) {
            std::cout << matrice_energetica[DAYS-1][i] << "W, ";
    }
    std::cout<<std::endl;
}












