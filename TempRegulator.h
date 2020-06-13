#pragma once
// Project headers
#include "schematic.h"
#include "settings.h"
// HAL headers
#include "HAL_ADC.h"
#include "HAL_PWM.h"
#include "HAL_Timer.h"

class TempRegulator{
public:
struct TR_state{
	// Read / write
	bool _enable;
	int _duty_cycle;
	bool _update;
	
	// Read only
	float _temp_volt;
	float _temp;
};

private:
// HAL classes
HAL_Timer &Timer;
HAL_ADC Adc;
HAL_PWM Pwm;

// Data
bool enable;
float temp_volt; // ADC reading of temp, in volts
float temperature; // temperature, in celsius
int duty_cycle; // duty cycle of PWM output
unsigned long last_time; // last time run, in ms

public:
TempRegulator(HAL_Timer &Timer_r);
void regulate();
// enable or disable regulator
void enable_regulation(){
	enable = true;
}
void disable_regulation(){
	enable = false;
}
// duty cycle
void set_duty_cycle(int val){
	// val is duty cycle, between 0 and 100
	duty_cycle = val;
}
int get_duty_cycle(){
	return duty_cycle;
}
// temperature
float get_temp_volt(){
	return temp_volt;
}
// Get / set state
void get_state(TR_state &state){
	// Allows quick extraction of info
	state._enable = enable;
	state._duty_cycle = duty_cycle;
	state._update = false;
	state._temp_volt = temp_volt;
	state._temp = temperature;
}
void set_state(TR_state &state){
	// Allows quick configuration of info
	// Only update if flag is set
	if(state._update){
		state._update = false;
		enable = state._enable;
		duty_cycle = state._duty_cycle;
	}
	
	// don't update temp_volt or temperature
}
};