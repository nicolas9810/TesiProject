
#include "strategia.h"
#include "defines.h"
#include <mutex>
#include <condition_variable>

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
    StrategiaGreedyPredizione();
    StrategiaGreedyPredizione(epever *dev);
    StrategiaGreedyPredizione& operator=(const StrategiaGreedyPredizione& other);
    virtual int strategia(float batteryCharge);
    virtual int getBestConfig(float predizione);
    void gestionePrevisioni();
    float misuraPotenza(epever* dev);
    void disattiva();
    void attiva();
    void setConsumi(int c);
};




