#include "TempRegulator.h"

TempRegulator::TempRegulator(HAL_Timer &Timer_r):
	Timer(Timer_r),
	Adc(SCH_TR_ADC_DIV, SCH_TR_ADC_REF, SET_TR_ADC_ADJ, SCH_TR_ADC_MASK),
	Pwm(SCH_TR_PWM_MODE, SCH_TR_PWM_CLK_SEL, SCH_TR_PWM_OUTPUT_MODE)
{
	last_time = 0;
	enable = true;
	duty_cycle = 0;
}

void TempRegulator::regulate(){
	// Only run if enough time has passed
	if(Timer.get_tick() - last_time > SET_TR_PERIOD){
		last_time = Timer.get_tick();
		temp_volt = SCH_TR_ADC_TO_VOLT(Adc.read(SCH_TR_ADC_TEMP_MUX));
		temperature = SCH_TR_VOLT_TO_TEMP(temp_volt);
		if(enable)
			// duty cycle depends on temperature
			duty_cycle = Pwm.set_duty_cycle(SET_TR_VOLT_TO_DUTY_CYCLE(temp_volt));
		else
			// duty cycle is manually adjusted
			duty_cycle = Pwm.set_duty_cycle(duty_cycle);
	}
}