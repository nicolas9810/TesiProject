#ifndef PREVISIONE_H
#define PREVISIONE_H

#include "GreenPlantModel.h"


double previsioneEnergiaDisponibile(int n,int min);
void inizializzaMatrice();
int setEnergia(double e,int min);
void nuovoGiorno();
void previsioneDelGiorno(int c,double en);
void media();
int setEnergia(int n,int e);
double gapk(int i);
double prodottoScalare(double v1[],double v2[],int j);
void inizializzaMatriceTest(GreenPlantModel model);
void printMatriceEnergetica();
void printPrevisioni();
void setEner(int i,int j,double e);

#endif //PREVISIONE_H