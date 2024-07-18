#ifndef STRATEGIA 
#define STRATEGIA 
#include "epever.h"
#include "tempo.h"


class Config {
    private:
    public:
        float consumo;
    };
class Strategia {
private:
protected:
    Config arrayConsumi[4];
    bool getBatteryStatus();
    float getBatteryCharge();
    epever* device;
    Tempo* tempo;


public:
    Strategia(epever* dev);
    epever* getDevice();
    void setConsumi(int c);
    virtual int strategia(float batteryStatus,int conf)=0;
    virtual void gestionePrevisioni()=0;
    void setTempo(Tempo *t);
    Tempo* getTempo();
};

#endif //STRATEGIA