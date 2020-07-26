#include "LoadRegulator.h"

LoadRegulator::LoadRegulator():
	// Call constructors for class members in initialization list
	LR_Timer(SET_LR_TIMER_NUMBER, SET_LR_TIMER_DIV, SET_LR_TIMER_TOP),
	LR_SPI(SCH_SPI_OP_MODE, SCH_SPI_CLK_SEL, SCH_SPI_BIT_ORDER, SCH_SPI_SS_DIR),
	LR_TWI(),
	LR_CUR_CONT_CS(SCH_MAX5216_CS_PORT, SCH_MAX5216_CS_PIN, SCH_MAX5216_CS_DIR, SCH_MAX5216_CS_VAL),
	LR_VMON_CS(SCH_ADS8685_CS_PORT, SCH_ADS8685_CS_PIN, SCH_ADS8685_CS_DIR, SCH_ADS8685_CS_VAL),
	current_monitor(LR_TWI, SCH_LTC2451_REF),
	current_control(LR_CUR_CONT_CS, LR_SPI, SCH_MAX5216_REF, MAX5216::OUT_GND_1K),
	volt_monitor(LR_VMON_CS, LR_SPI),
	DBG_LED(SET_LED_LR_PORT, SET_LED_LR_PIN, SET_LED_LR_ACTIVE),
	curr_buffer(),
	volt_buffer()
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
		// DEBUG: turn LED on
		DBG_LED.on();
		
		// enough time has passed that offset must be adjusted
		last_offset_time = cur_time;
		
		// measure current through load; only update on successful read
		float cur_mon_volt = 0;
		HAL_TWI::TWI_ERROR error = current_monitor.read(cur_mon_volt);
		if(error == HAL_TWI::TWI_NO_ERROR){
			// current read was successful
			measured_current = SCH_VOLT_TO_AMP(cur_mon_volt, cal_zero);
			
			// update ring buffer
			curr_buffer.push(measured_current);
			
			// increase offset by difference of desired and measured current
			// only update offset if new calculated offset is within limits
			offset += (desired_current - measured_current) * SET_LR_CUR_ERROR_SCALER;
			if(offset < SET_LR_OFFSET_MIN){
				offset = SET_LR_OFFSET_MIN;
			} else if(offset > SET_LR_OFFSET_MAX){
				offset = SET_LR_OFFSET_MAX;
			}
		}
	}
	
	// see if desired current & output needs to be updated
	if(cur_time - last_desired_time > SET_LR_DESIRED_UPDATE_PERIOD){
		// DEBUG: turn LED on
		DBG_LED.on();
		
		// enough time has passed that desired current and DAC output must be adjusted
		last_desired_time = cur_time;
		// measure voltage across load
		measured_voltage = SCH_ADS8685_GAIN * volt_monitor.read();
		// update ring buffer
		volt_buffer.push(measured_voltage);
		
		// calculate desired current and DAC output
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
				// adjust desired current based on voltage across load
				if(measured_voltage > target_voltage + SET_LR_CV_TOL)
					desired_current += SET_LR_CV_CUR_STEP;
				else if (measured_voltage < target_voltage - SET_LR_CV_TOL)
					desired_current -= SET_LR_CV_CUR_STEP;
				// limit desired current
				if(desired_current > SET_LR_DESIRED_CUR_MAX)
					desired_current = SET_LR_DESIRED_CUR_MAX;
				else if(desired_current < SET_LR_DESIRED_CUR_MIN)
					desired_current = SET_LR_DESIRED_CUR_MIN;
				break;
			case(OFF):
				desired_current = 0;
				break;
		}
		
		// Update DAC output
		if(desired_current == 0){
			// zero current is special case
			offset = 0; // prevent offset from wandering during zero current condition
			current_control.set_output(SCH_ZERO_AMP_VOLT);
		} else{
			current_control.set_output(SCH_AMP_TO_VOLT(desired_current + offset, cal_zero));
		}
	}
	
	// DEBUG: turn LED off
	DBG_LED.off();
}
void LoadRegulator::calibrate_zero(){
	// Sets cal_zero to average of several current monitor readings
	// Set output to zero current
	current_control.set_output(SCH_ZERO_AMP_VOLT);
	
	// Pause to let system stabilize
	unsigned long start_time = LR_Timer.get_tick();
	while(start_time - LR_Timer.get_tick() < SET_LR_CAL_DELAY_PERIOD);
	
	// Get average of several readings
	float sum = 0;
	HAL_TWI::TWI_ERROR error;
	for(int i = 0; i < SET_LR_CAL_AMOUNT + SET_LR_CAL_FLUSH_AMOUNT; i++){
		float temp_val = 0;
		error = current_monitor.read(temp_val);
		if(error == HAL_TWI::TWI_NO_ERROR){
			// Got reading; only use samples after flushing completes
			if(i >= SET_LR_CAL_FLUSH_AMOUNT)
				sum += temp_val;
		}else
			// Error occurred; try again
			i--;
	}
	
	// update cal_zero
	cal_zero = sum / SET_LR_CAL_AMOUNT;
}
