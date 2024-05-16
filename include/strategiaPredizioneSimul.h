
#include "strategia.h"
#include "defines.h"
#include <mutex>
#include <condition_variable>
#include "previsione.h"


class StrategiaPredizioneSimul:public Strategia{
private:

protected:
    // Config arrayConsumi[4];

public:

    GreenPlantModel model;;
    std::mutex mtx;
    std::condition_variable cv;
    bool attivo = false;

    bool giornoNuovo;
    bool giornoUno;
    int giorno;
    StrategiaPredizioneSimul();
    StrategiaPredizioneSimul(epever *dev);
    StrategiaPredizioneSimul& operator=(const StrategiaPredizioneSimul& other);
    virtual int strategia(float batteryCharge);
    virtual int getBestConfig(float predizione);
    void gestionePrevisioni();
    float misuraPotenza();
    void disattiva();
    void attiva();

    // void setConsumi(int c);
};




