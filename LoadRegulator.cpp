#include "LoadRegulator.h"

LoadRegulator::LoadRegulator():
	// Call constructors for class members in initialization list
	LR_Timer(SET_LR_TIMER_NUMBER, SET_LR_TIMER_DIV, SET_LR_TIMER_TOP),
	LR_SPI(SCH_SPI_OP_MODE, SCH_SPI_CLK_SEL),
	LR_TWI(),
	LR_CUR_CONT_CS(SCH_MAX5216_CS_PORT, SCH_MAX5216_CS_PIN, SCH_MAX5216_CS_DIR, SCH_MAX5216_CS_VAL),
	LR_VMON_CS(SCH_ADS8685_CS_PORT, SCH_ADS8685_CS_PIN, SCH_ADS8685_CS_DIR, SCH_ADS8685_CS_VAL),
	current_monitor(LR_TWI, SCH_LTC2451_REF),
	current_control(LR_CUR_CONT_CS, LR_SPI, SCH_MAX5216_REF),
	volt_monitor(LR_VMON_CS, LR_SPI)
{
	// Configure voltage monitor ADC
	volt_monitor.set_input_range(SCH_ADS8685_RANGE, SCH_ADS8685_REF);
	
	// Set initial resistance value
	target_resistance = SET_LR_INIT_RES;
	
	// Puts load regulator into innocuous state
	// Calibrates zero
	set_mode(OFF);
	regulate();
	set_target_current(0);
	calibrate_zero();
}
void LoadRegulator::regulate(){
	// Only run code if timer flag is set
	if(LR_Timer.get_int_flag()){
		LR_Timer.clear_int_flag();
		
		// Update measured current on valid read
		float cur_mon_volt = 0;
		HAL_TWI::TWI_ERROR error = current_monitor.read(cur_mon_volt);
		if(error == HAL_TWI::TWI_NO_ERROR){
			// Only update measured current on valid read
			measured_current = SCH_VOLT_TO_AMP(cur_mon_volt, cal_zero);
		}
		
		// Update measured voltage
		measured_voltage = SCH_ADS8685_GAIN * volt_monitor.read();
		
		// Update controls
		switch(op_mode){
			case(CC):
			case(CP):
			case(CR):
				// Adjust control current and update DAC output only when measured current is updated
				if(error == HAL_TWI::TWI_NO_ERROR){
					adjust_control_current();
					current_control.set_output(SCH_AMP_TO_VOLT(control_current, cal_zero));
				}
				break;
			case(CV):
				break;
			case(OFF):
			default:
				current_control.set_output(SCH_ZERO_AMP_VOLT);
				break;
		}
	}
}
void LoadRegulator::calibrate_zero(){
	// Sets cal_zero to average of several current monitor readings
	float sum = 0;
	HAL_TWI::TWI_ERROR error;
	for(int i = 0; i < SET_LR_CAL_AMOUNT; i++){
		float temp_val = 0;
		error = current_monitor.read(temp_val);
		if(error == HAL_TWI::TWI_NO_ERROR)
			// Got reading; use it
			sum += temp_val;
		else
			// Error occurred; try again
			i--;
	}
	cal_zero = sum / SET_LR_CAL_AMOUNT;
}
void LoadRegulator::adjust_control_current(){
	// adjusts control current based on operation mode
	switch(op_mode){
		case(CC):
			// In constant current mode, increase control current by difference of target and measured current
			control_current += (target_current - measured_current) * SET_LR_CUR_ERROR_SCALER;
			break;
		case(CP):
			// In constant power mode, increase control current by difference of (target power / measured voltage) and measured current
			control_current += (target_power/measured_voltage - measured_current) * SET_LR_CUR_ERROR_SCALER;
			break;
		case(CR):
			// In constant resistance mode, increase control current by difference of (measured voltage / target resistance) and measured current
			// only updates for positive resistance values
			if(target_resistance > 0)
				control_current += (measured_voltage/target_resistance - measured_current) * SET_LR_CUR_ERROR_SCALER;
		default:
			// Error case; do nothing
			break;
	}
}