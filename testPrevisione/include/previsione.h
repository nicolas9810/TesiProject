#ifndef PREVISIONE_H
#define PREVISIONE_H

#include "GreenPlantModel.h"


double previsioneEnergiaDisponibile(int n,int min);
void inizializzaMatrice();
int setEnergia(double e,int min);
void nuovoGiorno();
void previsioneDelGiorno(int c);
void media();
int setEnergia(int n,int e);
double gapk(int i);
double prodottoScalare(double v1[],double v2[]);
void inizializzaMatriceTest(GreenPlantModel model);
void printMatriceEnergetica();
void printPrevisioni();
void setEner(int i,int j,double e);
double recursiveEWMA(double alpha, int index,int giorno);

#endif //PREVISIONE_H