#include "LoadRegulator.h"

LoadRegulator::LoadRegulator(LTC2451 &LTC2451_r, MAX5216 &MAX5216_r, ADS8685 &ADS8685_r)
	: current_monitor(LTC2451_r), current_control(MAX5216_r), volt_monitor(ADS8685_r)
{
	// Puts load regulator into innocuous state
	// Calibrates zero
	set_mode(OFF);
	regulate();
	set_target_current(0);
	calibrate_zero();
}
void LoadRegulator::regulate(){
	// Update measured voltage and current
	float cur_mon_volt = 0;
	HAL_TWI::TWI_ERROR error = current_monitor.read(cur_mon_volt);
	if(error == HAL_TWI::TWI_NO_ERROR)
		// Only update measured current if no error occurred
		measured_current = SCH_VOLT_TO_AMP(cur_mon_volt, cal_zero);
	measured_voltage = SCH_ADS8685_GAIN * volt_monitor.read();
	
	// Update controls
	switch(op_mode){
		case(CC):
			current_control.set_output(SCH_AMP_TO_VOLT(target_current, cal_zero));
			break;
		case(CP):
			break;
		case(CR):
			break;
		case(CV):
			break;
		default:
			current_control.set_output(SCH_ZERO_AMP_VOLT);
			break;
	}
}
void LoadRegulator::calibrate_zero(){
	// Sets cal_zero to average of several current monitor readings
	float sum = 0;
	HAL_TWI::TWI_ERROR error;
	for(int i = 0; i < LR_CAL_AMOUNT; i++){
		float temp_val = 0;
		error = current_monitor.read(temp_val);
		if(error == HAL_TWI::TWI_NO_ERROR)
			// Got reading; use it
			sum += temp_val;
		else
			// Error occurred; try again
			i--;
	}
	cal_zero = sum / LR_CAL_AMOUNT;
}