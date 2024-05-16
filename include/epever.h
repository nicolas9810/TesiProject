
#include <modbus.h>
#include <string>
#include <bitset>
#include "chargeController.h"

/**
 * Class for epever MPPT Tracer1210AN. 
 * Class for the Mppt solar charge controller.
 */
class epever : public chargeController{
private:
    modbus_t *ctx;
    void clean_and_throw_error() const;
public:

    enum batteryStatus_t {
        VOLTAGE_FAULT=0,
        UNDER_VOLTAGE=1,
        OVER_VOLTAGE=2,
        OVER_DISCHARGE=3,
        VOLTAGE_NORMAL=4,
        TEMP_OK=5,
        OVER_TEMP=6,
        UNDER_TEMP=7,
        BATTERY_RESISTANCE_FAULT=8,
        WRONG_IDENTIFICATION_RATED_V=9,
        ALL_OK=10,
    };
    enum chargingEquipmentStatus_t {
        RUNNING=0,
        DEV_FAULT=1,
        IS_CHARGING=2,
        FLOAT=3,       //float charging
        BOOST=4,       //boost charging mode
        EQUALIZE=5,    //equalize charging mode
        PV_IN_SHORT=6, //pv input is short circuit
        LOAD_MOS_SHORT=7,
        LOAD_SHORT=8,
        LOAD_OVER_CURRENT=9,
        INPUT_OVER_CURRENT=10,
        ANTIREVERSE_MOS_SHORT=11,
        CHARGE_MOS_OPEN=12,
        CHARGE_MOS_SHRT=13,
        IN_V_NORMAL=14,
        NO_INPUT_POWER=15,
        HIGHER_INPUT_VOLTAGE=16,
        INPUT_VOLTAGE_ERROR=17,
    };

    enum dischargingEquipmentStatus_t {
        LOAD_ON=0,
        FAULT=1,
        OUTPUT_OVER_VOLTAGE=2,
        BOOST_OVER_VOLTAGE=3,       
        SHORT_HIGH_VOLTAGE=4,       
        INPUT_OVER_VOLTAGE=5,    
        OUTPUT_VOLTAGE_ABNORMAL=6, 
        UNABLE_STOP_DISCHARGING=7,
        UNABLE_DISCHARGE=8,
        SHORT_CIRCUIT=9,
        OUT_PW_LIGHT_LOAD=10,
        OUT_PW_MODERATE=11,
        OUT_PW_RATED=12,
        OUT_PW_OVERLOAD=13,
        INPUT_V_NORMAL=14,
        INPUT_VOLTAGE_LOW=15,
        INPUT_VOLTAGE_HIGH=16,
        NO_ACCESS=17,
    };

    /**
     * Epever class constructor.
     * @param device "/dev/ttyXRUSB0" 
     */
    epever(const std::string & device);
    virtual ~epever();

    //Real time data
    virtual float getArrayCurrent() const;
    virtual float getArrayVoltage() const override;
    float getArrayPower() const;
    virtual float getBatteryVoltage() const override;
    virtual float getChargeCurrent() const override;
    virtual float maxBatteryVoltageToday() const override;
    virtual float minBatteryVoltageToday() const override;
    virtual float getBatteryTemp() const override;




    
    /**
     * The percentage of battery's remaining capacity.
     */
    float getBatterySOC() const; 

    /**
     * Battery information.
     * @param en enum batteryStatus_t
     */
    bool getBatteryStatus(batteryStatus_t en) const;

    /**
     * Battery information, returns bitset, each bit or set of bits corresponds to a specific information.
     * @return Bitset. 
     * D15: 1-Wrong identification for rated voltage
     * D8: Battery inner resistance abnormal 1, normal 0
     * D7-D4: 00H Normal, 01H Over Temp.(Higher than the warning settings), 02H Low Temp.(Lower than the warning settings),
     * D3-D0: 00H Normal ,01H Over Voltage. , 02H Under Voltage, 03H Over discharge, 04H Fault
     */
    std::bitset<16> getBatteryStatus() const;


    /**
     * Charging equipment information.
     * @param en enum chargingEquipmentStatus_t 
     */
    bool getChargingEquipmentStatus(chargingEquipmentStatus_t en) const;

    /**
     * Battery information, returns bitset, each bit or set of bits corresponds to a specific information.
     * @return Bitset. 
     * D15-D14: Input voltage status. 00H normal, 01H No input power connected, 02H Higher input voltage , 03H Input voltage error.
     * D13: Charging MOSFET is short circuit.
     * D12: Charging or Anti-reverse MOSFET is open circuit.
     * D11: Anti-reverse MOSFET is short circuit.
     * D10: Input is over current.
     * D9: The load is over current.
     * D8: The load is short circuit.
     * D7: Load MOSFET is short circuit.
     * D6:Disequilibrium in three circuits.
     * D4: PV input is short circuit.
     * D3-D2: Charging status. 00H No charging,01H Float,02H Boost, 03H Equalization.
     * D1: 0 Normal, 1 Fault.
     * D0: 1 Running, 0 Standby.
     */
    std::bitset<16> getChargingEquipmentStatus() const;
    virtual float getLoadCurrent() const override;
    virtual float getLoadVoltage() const override;
    float getLoadPower() const;
    float getBatteryCurrent() const;


    /**
     * Disharging equipment information.
     * @param en enum dischargingEquipmentStatus_t 
     */
    bool getDischargingEquipmentStatus(dischargingEquipmentStatus_t en) const; 
    virtual float getHeatsinkTemp() const override;
    // ? realTimeClock() const;

    //Battery Parameters
    virtual float getHighVoltageDisconnect() const override;
    virtual void setHighVoltageDisconnect(float v)  override;
    virtual float getHighVoltageReconnect() const override;
    virtual void setHighVoltageReconnect(float v)  override;
    virtual float getLowVoltageDisconnect() const override;
    virtual void setLowVoltageDisconnect(float v)  override;
    virtual float getLowVoltageReconnect() const override;
    virtual void setLowVoltageReconnect(float v)  override;    
    float getBatteryCapacity() const;
    void setBatteryCapacity(float ah);
    float getEqualizeChargingV() const;
    void setEqualizeChargingV(float v);
    float getBoostChargingV() const;
    void setBoostChargingV(float v);
    float getFloatChargingV() const;
    void setFloatChargingV(float v);
    float getBoostReconnectCahargingV() const;
    void setBoostReconnectCahargingV(float v);
    float getDischargingLimitV() const;
    void setDischargingLimitV(float v);
    float getChargingLimitV() const;
    void setChargingLimitV(float v);
    float getBoosDuration() const;
    float getBatteryPower() const;

    
    /**
     * @param min minutes
     */
    void setBoosDuration(int min);
    float getEquilibriumDuration() const;
    
    /**
     * @param min minutes
     */
    void setEquilibriumDuration(int min);

    //Laod Parameters

    //Device Paraameters
    float getBatteryUpTempLimit() const;
    float getBatteryLwTempLimit() const;
    float getDeviceOverTemp() const;
    float getDeviceRecoveryTemp() const;
    
    /**
     * @param deg degrees °C
     */
    void setBatteryUpTempLimit(float deg);
   
    /**
     * @param deg degrees °C
     */
    void setBatteryLwTempLimit(float deg);
    
    /**
     * @param deg degrees °C
     */
    void setDeviceOverTemp(float deg);
    
    /**
     * @param deg degrees °C
     */
    void setDeviceRecoveryTemp(float deg);

    //switching values
    void chargingDeviceOn() const;
    void chargingDeviceOff() const;
    void LoadOn();
    void loadOff();
    void RestoreSystemDefaults();
    void loadTestModeOn();
    void loadTestModeOff();
};

