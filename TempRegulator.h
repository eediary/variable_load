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
bool is_enabled(){
	return enable;
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
};