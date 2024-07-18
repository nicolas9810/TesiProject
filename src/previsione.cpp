#include <ctime> 
#include "defines.h"
#include <iostream>
#include "previsione.h"
#include <thread>
#include <chrono>




// idealmente sono da aggiustare dinamenicamente
const float alfa = 0.6;
const int kappa = 4;

int oggi= DAYS-1;
int ieri = DAYS-2;

double matrice_energetica [DAYS][NN];
// int matrice_previsione_energetica [DAYS][NN];
double medie_energetiche [NN];
double previsioni[NN];

double previsioneEnergiaDisponibile(int n,int min){
    // time_t now = time(0); // get current date and time  
    // tm* ltm = localtime(&now);  
    // int hour= ltm->tm_hour;
    // int min= ltm->tm_min;
    // min=min+hour*60;
    int slot=min/FREQUENZA;
    double result;
    int c=0;
    /*for(int i=slot;i<slot+n && i<NN;i++){
        c++;
        result+=matrice_energetica[DAYS][i];
    } 
    return result/c;*/
    return matrice_energetica[DAYS][(slot+1)%24];
}


int setEnergia(double e,int min){
    // time_t now = time(0); // get current date and time  
    // tm* ltm = localtime(&now);  
    // int hour= ltm->tm_hour;
    // int min= ltm->tm_min;
    // min=min+hour*60;
    int slot=(min/60)-1;
    std::cout<< "slot numero "<<slot<< " "<< min  <<std::endl;
    std::cout << "set energia " << e << "W in slot " << slot << "  oggi " <<std::endl;

    matrice_energetica[oggi][slot]=e;
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
        for(int j=0;j<DAYS-1;j++){
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
        std::cout << "set energia " << e << "W in slot " << n<< "  oggi " <<std::endl;
        
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

double recursiveEWMA(float alpha, int index, int giorno) {
    // Caso base: se l'indice è zero, restituisci il primo valore dei dati
    if (giorno == 0) {
        return matrice_energetica[giorno][index];
    }
    // Caso ricorsivo: calcola l'EWMA per l'indice corrente
    return alpha * matrice_energetica[giorno][index] + (1 - alpha) * recursiveEWMA( alpha, index, giorno-1);
}

void previsioneDelGiorno(int c,double en){
    double per=(en-medie_energetiche[c])/medie_energetiche[c];
    std::cout << "percentuale variabile del "<< per <<"%"<<std::endl;
    for(int i = 0;i< NN;i++){
        if(i <c){
            previsioni[i]=0;
        }
        else{
            if(medie_energetiche[c]==0.0){
                previsioni[i]=0.0;
            }
            else{
                // double mediaSlot=medie_energetiche[i];
                double prev = recursiveEWMA(alfa,i,oggi-1);
                std::cout<< "slot "<< i << " "<< prev << " varia di "<< per*prev <<""<<std::endl;
                

                previsioni[i] = prev+ prev*per;
                // std::cout << "revisione "<< i <<" = "<< matrice_energetica[oggi][i] <<std::endl;}
            }
        }
    }
}



void printMatriceEnergetica(){
    for (int i = 0; i < DAYS; ++i) {
        for (int j = 0; j < NN; ++j) {
            std::cout << matrice_energetica[i][j] << " ";
            // if(matrice_energetica[i][j]==1){
            // std::cout << " i valori di i e j sono" << i<< " "<< j <<std::endl;

            // }
        }
        std::cout << std::endl;
    }
}

void printPrevisioni(){
    for (int i = 0; i < NN; ++i) {
            std::cout << previsioni[i] << "W, ";
    }
    std::cout<<std::endl;
}

void gestionePrevisioniVirtuale(Tempo *tempo, GreenPlantModel *modello){
    int giorno=-1;
    bool flag = false;

    //inizializzaMatrice();
    for(int i=0;i<DAYS;i++){
         int temp=i*24*60;
        for(int j=0;j<NN;j++){
            temp+=60;
            //tempo->incrementsMinutes(60);
            setEner(i,j,modello->getProducedPowerByTime(temp));
        }
    }
    while (true)
    {
        if(tempo->getDay()!=giorno){
            std::cout << "Nuovo giorno è il giorno" << tempo->getDay() <<std::endl;
            nuovoGiorno();
            flag=false;
            printMatriceEnergetica();
            std::cout << "calcolo le previsioni del giorno "<<tempo->getDay()<<std::endl;
            //previsioneDelGiorno(0);
            giorno=tempo->getDay();
            //tempo->incrementsMinutes(60);
            std::this_thread::sleep_for(std::chrono::seconds(1));

        }else{
            int min=tempo->getTimeInMin();
            int minDelGiorno=tempo->getMinutes();
            int oraDelGiorno=tempo->getHours();
            double misurazioni=modello->getProducedPowerByTime(min);
            std::cout << "Produced power at time " << min << " : " << misurazioni <<std::endl;
            setEnergia(misurazioni,tempo->getMinutes()+tempo->getHours()*60);
            if(misurazioni>0 && !flag){
                flag = true;
                
                int slot=(minDelGiorno+oraDelGiorno*60)/FREQUENZA;
                //previsioneDelGiorno(slot-1,misurazioni);
                printPrevisioni();
            }
            //tempo->incrementsMinutes(60);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if(misurazioni>0 && !flag){
                flag = true;
                int slot=(minDelGiorno+oraDelGiorno*60)/FREQUENZA;
                previsioneDelGiorno(slot,misurazioni);
            }
        }
        
    }
    
}












