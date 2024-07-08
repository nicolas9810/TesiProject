#include <iostream>
#include "epever.h"

#define BAUD 115200
#define PARITY 'NN'
#define DATA_BIT 8
#define STOP_BIT 1

// epever::epever(){}
epever::epever(const std::string &device)
{
    ctx= modbus_new_rtu(device.c_str(), BAUD, PARITY, DATA_BIT, STOP_BIT);
    if (ctx == NULL) {
        throw std::runtime_error("Unable to create the libmodbus context");
    }
    modbus_set_slave(ctx, 1);
    modbus_rtu_set_serial_mode(ctx,MODBUS_RTU_RS485);
    modbus_set_response_timeout(ctx, 3, 0);

    if (modbus_connect(ctx) == -1) {
        modbus_free(ctx);
        throw std::runtime_error("Connection failed.");
    }
}

epever::~epever()
{
    modbus_close(ctx);
    modbus_free(ctx);
}
void epever::clean_and_throw_error() const {
    modbus_free(ctx);
    throw std::runtime_error(modbus_strerror(errno));
}

float epever::getBatteryVoltage() const {
    uint16_t dest[16];
    // modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x331A, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
    

float epever::getLoadCurrent() const {
    uint16_t dest[16];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x310D, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
  
}

float epever::getLoadVoltage() const {
    uint16_t dest[16];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x310C, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }

}

//ottengo la potenza in uscita dalla batteria
float epever::getOutputPower() const {
    return epever::getLoadVoltage()*epever::getLoadCurrent();
}

float epever::getBatteryCurrentL() const {
    uint16_t dest[2];
    // modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x310E, 2, dest)==-1){
        clean_and_throw_error();
    }else{
        uint32_t result = (dest[0]<<16)+dest[1];
        return result;
    }
}

float epever::getBatteryCurrentStatus() const {
    uint16_t dest[2];
    // modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x310B, 2, dest)==-1){
        clean_and_throw_error();
    }else{
        uint32_t result = (dest[0]<<16)+dest[1];
        return result;
    }
}




float epever::getChargingPower() const {
    uint16_t dest[2];
    // modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3106, 2, dest)==-1){
        clean_and_throw_error();
    }else{
        uint32_t result = (dest[0]<<16)+dest[1];
        return result;
    }
}

float epever::getLoadPower() const {
    uint16_t dest[2];
    // modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x310E, 2, dest)==-1){
        clean_and_throw_error();
    }else{
        uint32_t result = (dest[0]<<16)+dest[1];
        return result;
    }
}

float epever::getDischargeCurrentEqui() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x310D, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01; 
    }
}
  
   
float epever::getChargeCurrent() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3101, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]; 
    }
}
float epever::getArrayVoltage() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3100, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]; 
    }
}
float epever::getArrayPower() const {
    uint16_t dest[2];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3102, 2, dest)==-1){
        clean_and_throw_error();
    }else{
        uint32_t result = (dest[0]<<16)+dest[1];
        return result; 
    }
}
    std::bitset<16> epever::getChargingEquipmentStatus() const {
    uint16_t dest;
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3202, 1, &dest)==-1){ 
        clean_and_throw_error();
    }else{ 
        std::bitset<16> result=dest; 
        return result;
    }
}

bool epever::getChargingEquipmentStatus(chargingEquipmentStatus_t en) const {
    uint16_t dest;
    bool result;
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3201, 1, &dest)==-1){
        clean_and_throw_error();
    }else{
            std::bitset<16> set=dest; //enum?
        switch (en)
        {
        case RUNNING:
            result=set.test(0);
            return result;
            break;
        case DEV_FAULT:
            result=set.test(1);
            return result;
            break;    
        case IS_CHARGING:
            result=(!set.test(2))&&(!set.test(3));
            return result;
            break;
        case FLOAT:
            result=(!set.test(2))&&(set.test(3));
            return result;
            break;
        case BOOST:
            result=(set.test(2))&&(!set.test(3));
            return result;
            break;
        case EQUALIZE:
            result=(set.test(2))&&(set.test(3));
            return result;
            break;
        case PV_IN_SHORT:
            result=set.test(4);
            return result;
            break;
        case LOAD_MOS_SHORT:
            result=set.test(7);
            return result;
            break;
        case LOAD_SHORT:
            result=set.test(8);
            return result;
            break;
        case LOAD_OVER_CURRENT:
            result=set.test(9);
            return result;
            break;
        case INPUT_OVER_CURRENT:
            result=set.test(10);
            return result;
            break;
        case ANTIREVERSE_MOS_SHORT:
            result=set.test(11);
            return result;
            break;
        case CHARGE_MOS_OPEN:
            result=set.test(12);
            return result;
            break;
        case CHARGE_MOS_SHRT:
            result=set.test(13);
            return result;
            break;
        case IN_V_NORMAL:
            result=(!set.test(14))&&(!set.test(15));
            return result;
            break;
        case NO_INPUT_POWER:
            result=(!set.test(14))&&(set.test(15));
            return result;
            break;
        case HIGHER_INPUT_VOLTAGE:
            result=(set.test(14))&&(!set.test(15));
            return result;
            break;
        case INPUT_VOLTAGE_ERROR:
            result=(set.test(14))&&(set.test(15));
            return result;
            break;
        default:
            break;
        }
    }
}


bool epever::getDischargingEquipmentStatus(dischargingEquipmentStatus_t en) const {
    uint16_t dest;
    bool result;
    //uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3202, 1, &dest)==-1){ 
        clean_and_throw_error();
    }else{ 
        std::bitset<16> set=dest; 
        switch (en)
        {
        case LOAD_ON:
            result=set.test(0);
            return result;
            break;
        case FAULT:
            result=set.test(1);
            return result;
            break;    
        case OUTPUT_OVER_VOLTAGE:
            result=set.test(4);
            return result;
            break;
        case BOOST_OVER_VOLTAGE:
            result=set.test(5);
            return result;
            break;
        case SHORT_HIGH_VOLTAGE:
            result=set.test(6);
            return result;
            break;
        case INPUT_OVER_VOLTAGE:
            result=set.test(7);
            return result;
            break;
        case OUTPUT_VOLTAGE_ABNORMAL:
            result=set.test(8);
            return result;
            break;
        case UNABLE_STOP_DISCHARGING:
            result=set.test(9);
            return result;
            break;
        case UNABLE_DISCHARGE:
            result=set.test(10);
            return result;
            break;
        case SHORT_CIRCUIT:
            result=set.test(11);
            return result;
            break;
        case OUT_PW_LIGHT_LOAD:
            result=(!set.test(12))&&(!set.test(13));
            return result;
            break;
        case OUT_PW_MODERATE:
            result=(!set.test(12))&&(set.test(13));
            return result;
            break;
        case OUT_PW_RATED:
            result=(set.test(12))&&(!set.test(13));
            return result;
            break;
        case OUT_PW_OVERLOAD:
            result=(set.test(12))&&(set.test(13));
            return result;
            break;
        case INPUT_V_NORMAL:
            result=(!set.test(14))&&(!set.test(15));
            return result;
            break;
        case INPUT_VOLTAGE_LOW:
            result=(!set.test(14))&&(set.test(15));
            return result;
            break;
        case INPUT_VOLTAGE_HIGH:
            result=(set.test(14))&&(!set.test(15));
            return result;
            break;
        case NO_ACCESS:
            result=(set.test(14))&&(set.test(15));
            return result;
            break;
        default:
            break;
        }
    }
}
float epever::getHeatsinkTemp() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3111, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0];  
    }
}


float epever::maxBatteryVoltageToday() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3302, 1, dest)==-1){ 
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
float epever::minBatteryVoltageToday() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3303, 1, dest)==-1){ 
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}

float epever::getBatteryTemp() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3110, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        float res=dest[0];
        return res/100;  
    }
}
    
float epever::getHighVoltageDisconnect() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9003, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}

void epever::setHighVoltageDisconnect(float v) {
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x9003, v*100)==-1){
        clean_and_throw_error();
    }
}

float epever::getHighVoltageReconnect() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9005, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}

void epever::setHighVoltageReconnect(float v)  {
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x9005, v*100)==-1){
        clean_and_throw_error();
    }
}

float epever::getLowVoltageDisconnect() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x900D, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}

void epever::setLowVoltageDisconnect(float v)  {
        //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x900D, v*100)==-1){
        clean_and_throw_error();
    }
}

float epever::getLowVoltageReconnect() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x900A, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}

void epever::setLowVoltageReconnect(float v) {
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x900A, v*100)==-1){
        clean_and_throw_error();
    }
}

float epever::getArrayCurrent() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3101, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
float epever::getBatterySOC() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x311A, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0];
    }
}
std::bitset<16> epever::getBatteryStatus() const {
    uint16_t dest;
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3200, 1, &dest)==-1){ 
        clean_and_throw_error();
    }else{ 
            std::bitset<16> result=dest; 
        return result;
}
}

bool epever::getBatteryStatus(batteryStatus_t en) const {
    uint16_t dest;
    bool result;
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x3200, 1, &dest)==-1){
        clean_and_throw_error();
    }else{
            std::bitset<16> set=dest; //enum?
        switch (en)
        {
        case VOLTAGE_FAULT:
            result= set.test(2)&&(!set.test(1))&&(!set.test(0));
            return result;
            break;
        case UNDER_VOLTAGE:
            result= set.test(1)&&(!set.test(0));
            return result;
            break;
        case OVER_VOLTAGE:
            result= (!set.test(1))&&set.test(0);
            return result;
            break;
        case OVER_DISCHARGE:
            result= set.test(1)&&set.test(0);
            return result;
            break;
        case VOLTAGE_NORMAL:
            result= (!set.test(1))&&(!set.test(0));
            return result;
            break;
        case TEMP_OK:
            result= (!set.test(4))&&(!set.test(5));
            return result;
            break;
        case OVER_TEMP:
            result= set.test(4)&&(!set.test(5));
            return result;
            break;
        case UNDER_TEMP:
            result= (!set.test(4))&&set.test(5);
            return result;
            break;
        case BATTERY_RESISTANCE_FAULT:
            result= set.test(8);
            return result;
            break;
        case WRONG_IDENTIFICATION_RATED_V:
            result= set.test(15);
            return result;
            break;
        case ALL_OK:
            result= set.none();
            return result;
            break;
        
        default:
            break;
        }
        //D3-D0: 00H Normal ,01H Over Voltage. , 02H Under Voltage, 03H Over discharge, 04H Fault
    }
}



float epever::getBatteryCapacity() const{
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9001, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0];
    }
}
void epever::setBatteryCapacity(float ah){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x9001, ah)==-1){
        clean_and_throw_error();
    }
}
float epever::getEqualizeChargingV() const {
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9006, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
void epever::setEqualizeChargingV(float v){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x9006, v*100)==-1){
        clean_and_throw_error();
    }
}
float epever::getBoostChargingV() const{
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9007, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
void epever::setBoostChargingV(float v){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x9007, v*100)==-1){
        clean_and_throw_error();
    }
}
float epever::getFloatChargingV() const{
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9008, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
void epever::setFloatChargingV(float v){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x9008, v*100)==-1){
        clean_and_throw_error();
    }
}
float epever::getBoostReconnectCahargingV() const{
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9009, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
void epever::setBoostReconnectCahargingV(float v){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x9009, v*100)==-1){
        clean_and_throw_error();
    }
}
float epever::getDischargingLimitV() const{
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x900E, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
void epever::setDischargingLimitV(float v){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x900E, v*100)==-1){
        clean_and_throw_error();
    }
}
float epever::getChargingLimitV() const{
        uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9004, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
void epever::setChargingLimitV(float v){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x9004, v*100)==-1){
        clean_and_throw_error();
    }
}
float epever::getBoosDuration() const{
        uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x906B, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0];
    }
}
void epever::setBoosDuration(int min){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x906B, min)==-1){
        clean_and_throw_error();
    }
}
float epever::getEquilibriumDuration() const{
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x906C, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0];
    }
}
void epever::setEquilibriumDuration(int min){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x906C, min)==-1){
        clean_and_throw_error();
    }
}
float epever::getBatteryUpTempLimit() const{
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9017, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
float epever::getBatteryLwTempLimit() const{
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9018, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
float epever::getDeviceOverTemp() const{
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x9019, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
float epever::getDeviceRecoveryTemp() const{
    uint16_t dest[1];
    //modbus_set_debug(ctx,TRUE);
    if(modbus_read_input_registers(ctx, 0x901A, 1, dest)==-1){
        clean_and_throw_error();
    }else{
        return dest[0]*0.01;
    }
}
void epever::setBatteryUpTempLimit(float deg){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x90017, deg*100)==-1){
        clean_and_throw_error();
    }
}
void epever::setBatteryLwTempLimit(float deg){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x9018, deg*100)==-1){
        clean_and_throw_error();
    }
}
void epever::setDeviceOverTemp(float deg){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x9019, deg*100)==-1){
        clean_and_throw_error();
    }
}
void epever::setDeviceRecoveryTemp(float deg){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_register(ctx, 0x901A, deg*100)==-1){
        clean_and_throw_error();
    }
}
void epever::chargingDeviceOn() const {
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_bit(ctx,0,0xFF00)==-1){
        clean_and_throw_error();
    }else{
            
    }
}
void epever::chargingDeviceOff() const {
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_bit(ctx,0,0)==-1){
        clean_and_throw_error();
    }else{
            
    }
}
void epever::LoadOn(){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_bit(ctx,6,0xFF00)==-1){
        clean_and_throw_error();
    }else{
            
    }
}
void epever::loadOff(){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_bit(ctx,6,0)==-1){
        clean_and_throw_error();
    }else{
            
    }
}
void epever::RestoreSystemDefaults(){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_bit(ctx,13,0xFF00)==-1){
        clean_and_throw_error();
    }else{
            
    }
}
void epever::loadTestModeOn(){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_bit(ctx,5,0xFF00)==-1){
        clean_and_throw_error();
    }else{
            
    }
}
void epever::loadTestModeOff(){
    //modbus_set_debug(ctx,TRUE);
    if(modbus_write_bit(ctx,5,0)==-1){
        clean_and_throw_error();
    }else{
            
    }
}