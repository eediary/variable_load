#include "TempRegulator.h"

TempRegulator::TempRegulator(HAL_Timer &Timer_r):
	Timer(Timer_r),
	Adc(SCH_TR_ADC_DIV, SCH_TR_ADC_REF, SET_TR_ADC_ADJ, SCH_TR_ADC_MASK),
	Pwm(SCH_TR_PWM_MODE, SCH_TR_PWM_CLK_SEL, SCH_TR_PWM_OUTPUT_MODE),
	DBG_LED(SET_LED_TR_PORT, SET_LED_TR_PIN, SET_LED_TR_ACTIVE)
{
	last_time = 0;
	enable = true;
	target_duty_cycle = 0;
}

void TempRegulator::regulate(){
	// Only run if enough time has passed
	unsigned long cur_time = Timer.get_tick();
	if(cur_time - last_time > SET_TR_PERIOD){
		// DEBUG: turn LED on
		DBG_LED.on();
		
		// sufficient time has passed to run loop
		last_time = cur_time;
		
		// Convert ADC to voltage, then voltage to temperature
		temperature = SCH_TR_VOLT_TO_TEMP(SCH_TR_ADC_TO_VOLT(Adc.read(SCH_TR_ADC_TEMP_MUX)));
		
		// set duty cycle
		if(enable)
			// use temperature to determine duty cycle
			set_duty_cycle(SET_TR_TEMP_TO_DUTY_CYCLE(temperature));
		else
			// duty cycle is manually adjusted
			set_duty_cycle(target_duty_cycle);
	}
	
	// DEBUG: turn LED off
	DBG_LED.off();
}