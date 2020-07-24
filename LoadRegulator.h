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

struct LR_state 
{
	// Read / write
	operation_mode _op_mode;
	float _target_current;
	float _target_power;
	float _target_resistance;
	float _target_voltage;
	bool _update;
	
	// Read only
	float _measured_current;
	float _measured_voltage;
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
float target_current; // target current in CC mode, in amps
float target_power; // target power in CP mode, in watts
float target_resistance; // target resistance in CR mode, in ohms
float target_voltage; // target voltage in CV mode, in volts
float measured_current; // measured current going through load, in amps
float desired_current; // desired current through load
float offset; // offset for accurate current
float measured_voltage; // in volts
// misc
unsigned long last_cur_time; // last time current was measured; in ms
unsigned long last_desired_time; // last time desired was updated
unsigned long last_offset_time; // last time offset was updated
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
}
float get_target_current(){
	// returns target current in amps
	return target_current;
}
float get_measured_current(){
	// returns measured current in amps
	return measured_current;
}
float get_desired_current(){
	return desired_current;
}
float get_offset(){
	return offset;
}
// Power
void set_target_power(float val){
	// val is in watts
	target_power = val;
}
float get_target_power(){
	return target_power;
}
// resistance
void set_target_resistance(float val){
	// val is in ohms
	// only updates if val is greater than 0
	if(val > 0)
		target_resistance = val;
}
float get_target_resistance(){
	return target_resistance;
}
// Voltage
float get_measured_voltage(){
	// returns measured volt in volts
	return measured_voltage;
}
void set_target_voltage(float val){
	// val is in volts
	target_voltage = val;
}
float get_target_voltage(){
	return target_voltage;
}
// Set / get state
void get_state(LR_state &state){
	// Updates members of provided struct
	// Allows quick extraction of info
	state._op_mode = op_mode;
	state._target_current = target_current;
	state._target_power = target_power;
	state._target_resistance = target_resistance;
	state._target_voltage = target_voltage;
	state._update = false;
	state._measured_current = measured_current;
	state._measured_voltage = measured_voltage;
}
void set_state(LR_state &state){
	// Only uses writable members
	// Allows quick configuration of instance
	// Update if flag is set to update
	if(state._update){
		state._update = false;
		op_mode = state._op_mode;
		target_current = state._target_current;
		target_power = state._target_power;
		target_resistance = state._target_resistance;
		target_voltage = state._target_voltage;
	}
	
	// Does not update measured_current, measured_voltage or control_cur
}
};