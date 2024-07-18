#ifndef STRATEGIAVIRTUALEPREDIZIONE_H
#define STRATEGIAVIRTUALEPREDIZIONE_H

#include "strategia.h"
#include "defines.h"
#include <mutex>
#include <condition_variable>
#include "tempo.h" 
#include "GreenPlantModel.h"

class StrategiaVirtualePredizione:public Strategia{
private:

protected:
    // Config arrayConsumi[4];

public:


    std::mutex mtx;
    std::condition_variable cv;
    bool attivo = false;

    bool giornoNuovo;
    bool giornoUno;
    Tempo *time;

    GreenPlantModel *modelloPannello;

    
    StrategiaVirtualePredizione();
    StrategiaVirtualePredizione(epever *dev,Tempo *t);
    StrategiaVirtualePredizione& operator=(const StrategiaVirtualePredizione& other);
    // StrategiaVirtualePredizione(const StrategiaVirtualePredizione& other) = default;
    // StrategiaVirtualePredizione(StrategiaVirtualePredizione&& other) noexcept;
    // StrategiaVirtualePredizione& operator=(const StrategiaVirtualePredizione& other);
    // StrategiaVirtualePredizione& operator=(StrategiaVirtualePredizione&& other) noexcept;


    virtual int strategia(float batteryCharge,int conf);
    virtual int getBestConfig(float predizione);
    void gestionePrevisioni();
    float misuraPotenza(epever* dev);
    void disattiva();
    void attiva();
    void setConsumi(int c);
    // int getTimeInMin();
};

#endif //STRATEGIAVIRTUALEPREDIZIONE_H


