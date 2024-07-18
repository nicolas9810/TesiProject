#ifndef STRATEGIAGREEDYPREDIZIONE_H
#define STRATEGIAGREEDYPREDIZIONE_H

#include "strategia.h"
#include "defines.h"
#include <mutex>
#include <condition_variable>
#include "tempo.h"

class StrategiaGreedyPredizione:public Strategia{
private:

protected:
    // Config arrayConsumi[4];

public:


    std::mutex mtx;
    std::condition_variable cv;
    bool attivo = false;

    bool giornoNuovo;
    bool giornoUno;
    Tempo *tempo;
    StrategiaGreedyPredizione();
    StrategiaGreedyPredizione(epever *dev,Tempo *t);
    StrategiaGreedyPredizione& operator=(const StrategiaGreedyPredizione& other);
    virtual int strategia(float batteryCharge,int conf);
    virtual int getBestConfig(float predizione);
    void gestionePrevisioni();
    float misuraPotenza(epever* dev);
    void disattiva();
    void attiva();
    void setConsumi(int c);
    int getTimeInMin();
};

#endif //STRATEGIAGREEDYPREDIZIONE_H


