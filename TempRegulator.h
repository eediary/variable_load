#pragma once
// Project headers
#include "schematic.h"
#include "settings.h"
// HAL headers
#include "HAL_ADC.h"
#include "HAL_PWM.h"

class TempRegulator{
public:

private:
// HAL classes
HAL_ADC Adc;
HAL_PWM Pwm;

// Data
bool enable;
float temp_volt;
float cur_temp;
int duty_cycle; // duty cycle of PWM output

public:
TempRegulator();
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
};