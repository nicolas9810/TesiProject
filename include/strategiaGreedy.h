#include "strategia.h"
#include "defines.h"
using namespace std;

class StrategiaGreedy:public Strategia{
private:

public:
    StrategiaGreedy(epever *dev);
    StrategiaGreedy();
    virtual int strategia(float batteryCharge);

};