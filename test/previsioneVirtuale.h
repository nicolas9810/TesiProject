#ifndef PREVISIONE_VIRTUALE_H
#define PREVISIONE_VIRTUALE_H

#include "GreenPlantModel.h"
#include "tempo.h"


float previsioneEnergiaDisponibile(int n,Tempo *t);
void inizializzaMatrice();
int setEnergia(float e,Tempo *t);
void nuovoGiorno();
void previsioneDelGiorno(int c);
void media();
int setEnergia(int n,int e);
float gapk(int i);
float prodottoScalare(float v1[],float v2[]);
void inizializzaMatriceTest(GreenPlantModel model);

#endif //PREVISIONE _VIRTUALE_H