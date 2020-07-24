#include "LoadRegulator.h"

LoadRegulator::LoadRegulator():
	// Call constructors for class members in initialization list
	LR_Timer(SET_LR_TIMER_NUMBER, SET_LR_TIMER_DIV, SET_LR_TIMER_TOP),
	LR_SPI(SCH_SPI_OP_MODE, SCH_SPI_CLK_SEL, SCH_SPI_BIT_ORDER, SCH_SPI_SS_DIR),
	LR_TWI(),
	LR_CUR_CONT_CS(SCH_MAX5216_CS_PORT, SCH_MAX5216_CS_PIN, SCH_MAX5216_CS_DIR, SCH_MAX5216_CS_VAL),
	LR_VMON_CS(SCH_ADS8685_CS_PORT, SCH_ADS8685_CS_PIN, SCH_ADS8685_CS_DIR, SCH_ADS8685_CS_VAL),
	current_monitor(LR_TWI, SCH_LTC2451_REF),
	current_control(LR_CUR_CONT_CS, LR_SPI, SCH_MAX5216_REF),
	volt_monitor(LR_VMON_CS, LR_SPI)
{
	// Enable timer interrupt
	LR_Timer.enable_int();
	
	// Configure voltage monitor ADC
	volt_monitor.set_input_range(SCH_ADS8685_RANGE, SCH_ADS8685_REF);
	
	// Set initial target values
	target_current = SET_LR_INIT_CUR;
	target_power = SET_LR_INIT_POW;
	target_resistance = SET_LR_INIT_RES;
	target_voltage = SET_LR_INIT_V;
	
	// Set initial times
	last_cur_time = 0;
	last_offset_time = 0;
	last_desired_time = 0;
	
	// Set control parameters to 0
	desired_current = 0;
	offset = 0;
	
	// Set initial mode to OFF
	set_mode(OFF);
}
void LoadRegulator::regulate(){
	// update offset and desired as necessary, then update DAC output
	
	// get current time
	unsigned long cur_time = LR_Timer.get_tick();
	
	// see if offset needs to be updated
	if(cur_time - last_offset_time > SET_LR_OFFSET_UPDATE_PERIOD){
		// enough time has passed that offset must be adjusted
		last_offset_time = cur_time;
		
		// measure current through load; only update on successful read
		float cur_mon_volt = 0;
		HAL_TWI::TWI_ERROR error = current_monitor.read(cur_mon_volt);
		if(error == HAL_TWI::TWI_NO_ERROR){
			// current read was successful
			measured_current = SCH_VOLT_TO_AMP(cur_mon_volt, cal_zero);
			
			// increase offset by difference of desired and measured current
			// only update offset if new calculated offset is within limits
			float new_offset = offset + (desired_current - measured_current);
			if((new_offset > SET_LR_OFFSET_MIN) && (new_offset < SET_LR_OFFSET_MAX))
				offset = new_offset;
		}
	}
	
	// see if desired current & output needs to be updated
	if(cur_time - last_desired_time > SET_LR_DESIRED_UPDATE_PERIOD){
		// enough time has passed that desired current and DAC output must be adjusted
		last_desired_time = cur_time;
		// measure voltage across load
		measured_voltage = SCH_ADS8685_GAIN * volt_monitor.read();
		
		// calculate desired current
		switch(op_mode){
			case(CC):
				desired_current = target_current;
				break;
			case(CP):
				desired_current = target_power / measured_voltage;
				break;
			case(CR):
				desired_current = measured_voltage / target_resistance;
				break;
			case(CV):
				// TODO: figure out desired current for CV
				desired_current = desired_current;
				break;
			case(OFF):
				desired_current = 0;
				break;
		}
		
		// update DAC output
		current_control.set_output(SCH_AMP_TO_VOLT(desired_current + offset, cal_zero));
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
