#pragma once

#include "schematic.h"

#include <util/delay.h>
#include "ADS8685.h"
#include "LTC2451.h"
#include "MAX5216.h"
#include "RBuffer.h"

#define LR_CAL_AMOUNT 10

class LoadRegulator{
public:
enum operation_mode{
	CC, CP, CR, CV, OFF
};

private:
LTC2451 &current_monitor;
MAX5216 &current_control;
ADS8685 &volt_monitor;
float cal_zero; // voltage at zero current
float target_current;
float measured_current; // in amps
float measured_voltage; // in volts
operation_mode op_mode;

public:
LoadRegulator(LTC2451 &LTC2451_r, MAX5216 &MAX5216_r, ADS8685 &ADS8685_r);
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

// Voltage
float get_measured_voltage(){
	// returns measured volt in volts
	return measured_voltage;
}

};