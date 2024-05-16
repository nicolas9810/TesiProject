#ifndef STRATEGIA 
#define STRATEGIA 
#include "epever.h"


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

public:
    Strategia(epever* dev);
    epever* getDevice();
    void setConsumi(int c);
    virtual int strategia(float batteryStatus)=0;
};

#endif