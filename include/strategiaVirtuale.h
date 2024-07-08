#ifndef STRATEGIAVIRTUALE_H
#define STRATEGIAVIRTUALE_H
#include "strategia.h"
#include "defines.h"
#include "GreenPlantModel.h"
#include "tempo.h" 
#include "tempo.h" 

using namespace std;

class StrategiaVirtuale:public Strategia{
private:

public:
    //StrategiaVirtuale(epever *dev,Tempo *t);
    StrategiaVirtuale(epever *dev,Tempo *t);

    StrategiaVirtuale();
    virtual int strategia(float batteryCharge);
    GreenPlantModel *modelloPannello;
    void gestioneTempo();
    Tempo *tempo;
    Tempo *time;



};


#endif //STRATEGIAVIRTUALE_H