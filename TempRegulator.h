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
	int _target_duty_cycle;
	bool _update;
	
	// Read only
	float _temp;
	int _current_duty_cycle;
};

private:
// HAL classes
HAL_Timer &Timer;
HAL_ADC Adc;
HAL_PWM Pwm;

// Data
bool enable;
float temperature; // temperature, in celsius
int target_duty_cycle; // desired duty cycle
int current_duty_cycle; // actual duty cycle
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
int set_duty_cycle(int val){
	// update target_duty_cycle
	// update PWM output
	// return current duty cycle
	target_duty_cycle = val;
	current_duty_cycle = Pwm.set_duty_cycle(target_duty_cycle);
	return current_duty_cycle;
}
int get_duty_cycle(){
	return current_duty_cycle;
}
// temperature
float get_temp(){
	return temperature;
}
// Get / set state
void get_state(TR_state &state){
	// Allows quick extraction of info
	state._enable = enable;
	state._target_duty_cycle = target_duty_cycle;
	state._update = false;
	state._temp = temperature;
	state._current_duty_cycle = current_duty_cycle;
}
void set_state(TR_state &state){
	// Allows quick configuration of info
	// Only update if flag is set
	if(state._update){
		state._update = false;
		enable = state._enable;
		target_duty_cycle = state._target_duty_cycle;
	}
	
	// don't update temp_volt, temperature or current_duty_cycle
}
};