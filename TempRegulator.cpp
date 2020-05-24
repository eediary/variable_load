#include "TempRegulator.h"

TempRegulator::TempRegulator():
	Adc(SCH_TR_ADC_DIV, SCH_TR_ADC_REF, SET_TR_ADC_ADJ, SCH_TR_ADC_MASK),
	Pwm(SCH_TR_PWM_MODE, SCH_TR_PWM_CLK_SEL, SCH_TR_PWM_OUTPUT_MODE){
	enable = true;
	duty_cycle = 0;
}

void TempRegulator::regulate(){
	temp_volt = SCH_TR_ADC_TO_VOLT(Adc.read(SCH_TR_ADC_TEMP_MUX));
	if(enable)
		// duty cycle depends on temperature
		duty_cycle = Pwm.set_duty_cycle(SET_TR_VOLT_TO_DUTY_CYCLE(temp_volt));
	else
		// duty cycle is manually adjusted
		duty_cycle = Pwm.set_duty_cycle(duty_cycle);
}