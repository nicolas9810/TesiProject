#ifndef CHARGECONTROLLER_H
#define CHARGECONTROLLER_H
class chargeController
{
private:
    
public:
    virtual ~chargeController() {}

    //--Array information
    virtual float getChargeCurrent() const = 0; 
    virtual float getArrayVoltage() const = 0;
    //virtual float getArrayPower() const = 0;
    //virtual float getArrayStatus() const = 0;
    
    //--Battery information
    virtual float getBatteryVoltage() const = 0;
    virtual float maxBatteryVoltageToday() const = 0;
    virtual float minBatteryVoltageToday() const = 0;    
    virtual float getBatteryTemp() const = 0; 
    virtual float getHighVoltageDisconnect() const =0;
    virtual void setHighVoltageDisconnect(float v) =0;
    virtual float getHighVoltageReconnect() const =0;
    virtual void setHighVoltageReconnect(float v)  =0;
    virtual float getLowVoltageDisconnect() const =0;
    virtual void setLowVoltageDisconnect(float v) =0;
    virtual float getLowVoltageReconnect() const =0;
    virtual void setLowVoltageReconnect(float v) =0;
    
    //--Load information
    virtual float getLoadCurrent() const = 0; 
    virtual float getLoadVoltage() const = 0; 
    //virtual float getLoadPower() const = 0;
    //virtual float getLoadStatus() const = 0; 

    // --Controller Information
    // Device Temperature
    virtual float getHeatsinkTemp() const = 0; 
          


 };

#endif