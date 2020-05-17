#pragma once
// Project headers
#include "schematic.h"
#include "settings.h"
// HAL headers
#include "HAL_Timer.h"
#include "HAL_SPI.h"
#include "HAL_TWI.h"
#include "HAL_GPIO.h"
// Device headers
#include "ADS8685.h"
#include "LTC2451.h"
#include "MAX5216.h"
#include "RBuffer.h"

class LoadRegulator{
public:
enum operation_mode{
	CC, CP, CR, CV, OFF
};

private:
// HAL classes
HAL_Timer LR_Timer;
HAL_SPI LR_SPI;
HAL_TWI LR_TWI;
HAL_GPIO LR_CUR_CONT_CS;
HAL_GPIO LR_VMON_CS;
// Device level classes
LTC2451 current_monitor;
MAX5216 current_control;
ADS8685 volt_monitor;
// Data
float cal_zero; // voltage at zero current
float target_current; // desired current going through load, in amps
float measured_current; // current going through load, in amps
float control_current; // controlled variable adjusted to get target current, in amps
float measured_voltage; // in volts
operation_mode op_mode;

public:
LoadRegulator();
void regulate();
void calibrate_zero();

// Operation Mode
void set_mode(operation_mode mode){
	op_mode = mode;
}
operation_mode get_mode(){
	return op_mode;
}

// Current
void set_target_current(float val){
	// val is in amps
	// control current starts same as target current, adjusted in regulate method
	target_current = val;
	control_current = val;
}
float get_target_current(){
	// returns target current in amps
	return target_current;
}
float get_measured_current(){
	// returns measured current in amps
	return measured_current;
}
float get_control_current(){
	// returns control current, in amps
	return control_current;
}
void adjust_control_current(){
	// adjusts control current based on error
	control_current += (target_current - measured_current) * SET_LR_CUR_ERROR_SCALER;
}

// Voltage
float get_measured_voltage(){
	// returns measured volt in volts
	return measured_voltage;
}

};