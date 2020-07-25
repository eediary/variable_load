#include "Debugger.h"

Debugger::Debugger(LoadRegulator &LoadRegulator_r, TempRegulator &TempRegulator_r, HAL_Timer &Timer_r):
	LR_r(LoadRegulator_r),
	TR_r(TempRegulator_r),
	Timer(Timer_r),
	Serial(SET_DEBUGGER_BAUD, SCH_UART_MODE, SET_UART_DATA_BITS, SET_UART_PARITY, SET_UART_STOP_BITS),
	DBG_LED(SET_LED_DB_PORT, SET_LED_DB_PIN, SET_LED_DB_ACTIVE)
{
	// Initialize last time
	last_time = 0;
	
	// Set up serial
	Serial.enable_rx_int();
	Serial.send_string("\r\nDebugger running...\r\n");
}

void Debugger::run_debugger(){
	unsigned long cur_time = Timer.get_tick();
	if(cur_time - last_time > SET_DEBUGGER_PERIOD){
		// DEBUG: turn LED on
		DBG_LED.on();
		
		// Sufficient time has passed; run debugger
		last_time = cur_time;
		
		// Update values to report
		float LR_measured_voltage = LR_r.get_measured_voltage();
		float LR_measured_current = LR_r.get_measured_current();
		float LR_desired_current = LR_r.get_desired_current();
		float LR_offset = LR_r.get_offset();
	
		// Construct string
		switch(LR_r.get_mode()){
			case(LoadRegulator::CC):
				// Mode
				strcpy(char_tx_buffer, "CC, ");
				// Measured voltage
				append_float(LR_measured_voltage);
				strcat(char_tx_buffer, " V, ");
				// Measured current
				append_float(LR_measured_current);
				strcat(char_tx_buffer, " A / ");
				// Desired current
				append_float(LR_desired_current);
				strcat(char_tx_buffer, " A (");
				// Offset current
				append_float(LR_offset);
				strcat(char_tx_buffer, " A)");
				break;
			case(LoadRegulator::CP):
				// Mode
				strcpy(char_tx_buffer, "CP, ");
				// Measured voltage
				append_float(LR_measured_voltage);
				strcat(char_tx_buffer, " V, ");
				// Measured power
				append_float(LR_measured_voltage * LR_measured_current);
				strcat(char_tx_buffer, " W / ");
				// Target power
				append_float(LR_r.get_target_power());
				strcat(char_tx_buffer, " W, ");
				// Desired current
				append_float(LR_desired_current);
				strcat(char_tx_buffer, " A (");
				// Offset current
				append_float(LR_offset);
				strcat(char_tx_buffer, " A)");
				break;
			case(LoadRegulator::CR):
				// Mode
				strcpy(char_tx_buffer, "CR, ");
				// Measured voltage
				append_float(LR_measured_voltage);
				strcat(char_tx_buffer, " V, ");
				// Measured resistance
				append_float(LR_measured_voltage / LR_measured_current);
				strcat(char_tx_buffer, " R / ");
				// Target resistance
				append_float(LR_r.get_target_resistance());
				strcat(char_tx_buffer, " R, ");
				// Desired current
				append_float(LR_desired_current);
				strcat(char_tx_buffer, " A (");
				// Offset current
				append_float(LR_offset);
				strcat(char_tx_buffer, " A)");
				break;
			case(LoadRegulator::CV):
				// Mode
				strcpy(char_tx_buffer, "CV, ");
				// Measured voltage
				append_float(LR_measured_voltage);
				strcat(char_tx_buffer, " V / ");
				// Target voltage
				append_float(LR_r.get_target_voltage());
				strcat(char_tx_buffer, " V, ");
				// Measured current
				append_float(LR_measured_current);
				strcat(char_tx_buffer, " A, ");
				// Desired current
				append_float(LR_desired_current);
				strcat(char_tx_buffer, " A (");
				// Offset current
				append_float(LR_offset);
				strcat(char_tx_buffer, " A)");
				break;
			case(LoadRegulator::OFF):
				// Mode
				strcpy(char_tx_buffer, "OFF, ");
				// Measured voltage
				append_float(LR_measured_voltage);
				strcat(char_tx_buffer, " V");
				break;
		}
		// Duty cycle
		strcat(char_tx_buffer, " | ");
		append_duty_cycle();
		strcat(char_tx_buffer, " %\r\n");
		
		// Transmit strings
		Serial.send_string_int(char_tx_buffer);
	
		// Check input
		if(Serial.read_rx_buffer(char_rx_buffer, SET_UART_RX_BUFFER_SIZE)){
			if(char_rx_buffer[0] == 'c')
				// Put LR into constant current mode
				LR_r.set_mode(LoadRegulator::CC);
			else if(char_rx_buffer[0] == 'p')
				// Put LR into constant power mode
				LR_r.set_mode(LoadRegulator::CP);
			else if(char_rx_buffer[0] == 'r')
				// Put LR into constant resistance mode
				LR_r.set_mode(LoadRegulator::CR);
			else if(char_rx_buffer[0] == 'v')
				// Put LR into constant voltage mode
				LR_r.set_mode(LoadRegulator::CV);
			else if(char_rx_buffer[0] == 'o')
				// Put LR into OFF mode
				LR_r.set_mode(LoadRegulator::OFF);
			else if((char_rx_buffer[0] >= 48) && (char_rx_buffer[0] <= 57))
				// Input is number; adjust set current, power or resistance
				switch(LR_r.get_mode()){
					case(LoadRegulator::CC):
						LR_r.set_target_current(atof(char_rx_buffer));
						break;
					case(LoadRegulator::CP):
						LR_r.set_target_power(atof(char_rx_buffer));
						break;
					case(LoadRegulator::CR):
						LR_r.set_target_resistance(atof(char_rx_buffer));
						break;
					case(LoadRegulator::CV):
						LR_r.set_target_voltage(atof(char_rx_buffer));
						break;
					default:
						// do nothing
						break;
				}
			else
				// Invalid input
				// Do nothing
				Serial.send_string("Invalid input.\r\n");
		}
	
	// DEBUG: turn LED off
	DBG_LED.off();
	}
}

void Debugger::append_float(float val){
	// appends float to tx buffer
	char temp_buffer[SET_UART_TEMP_BUFFER_SIZE];
	dtostrf(val, SET_DEBUGGER_FP_WIDTH, SET_DEBUGGER_FP_DECIMALS, temp_buffer);
	strcat(char_tx_buffer, temp_buffer);
}

void Debugger::append_duty_cycle(){
	// append temperature regulator duty cycle to tx buffer
	char temp_buffer[SET_UART_TEMP_BUFFER_SIZE];
	itoa(TR_r.get_duty_cycle(), temp_buffer, 10);
	strcat(char_tx_buffer, temp_buffer);
}