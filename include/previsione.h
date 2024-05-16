#include "modelloPannello.h"


float previsioneEnergiaDisponibile(int n);
void inizializzaMatrice();
int setEnergia(float e);
void nuovoGiorno();
void previsioneDelGiorno(int c);
void media();
int setEnergia(int n,int e);
float gapk(int i);
float prodottoScalare(float v1[],float v2[]);
void inizializzaMatriceTest(GreenPlantModel model);