#include "Debugger.h"

Debugger::Debugger(long baud_rate, LoadRegulator &LoadRegulator_r, TempRegulator &TempRegulator_r, HAL_Timer &Timer_r):
	LR_r(LoadRegulator_r),
	TR_r(TempRegulator_r),
	Timer(Timer_r),
	Serial(baud_rate, SCH_UART_MODE, SET_UART_DATA_BITS, SET_UART_PARITY, SET_UART_STOP_BITS),
	LED_G(SCH_LED_GREEN_PORT, SCH_LED_GREEN_PIN, SCH_LED_GREEN_ACTIVE),
	LED_B(SCH_LED_BLUE_PORT, SCH_LED_BLUE_PIN, SCH_LED_BLUE_ACTIVE),
	LED_Y(SCH_LED_YELLOW_PORT, SCH_LED_YELLOW_PIN, SCH_LED_YELLOW_ACTIVE),
	LED_R(SCH_LED_RED_PORT, SCH_LED_RED_PIN, SCH_LED_RED_ACTIVE)
{
	// Initialize last time
	last_time = 0;
	
	// Set up serial
	Serial.enable_rx_int();
	Serial.send_string("\r\nDebugger running...\r\n");
}

void Debugger::run_debugger(){
	if(Timer.get_tick() - last_time > SET_DEBUGGER_PERIOD){
		// Sufficient time has passed; run debugger
		last_time = Timer.get_tick();
		
		// Update values to report
		float LR_measured_voltage = LR_r.get_measured_voltage();
		float LR_measured_current = LR_r.get_measured_current();
		float LR_control_current = LR_r.get_control_current();
	
		// Print message
		LED_G.on();
		// Message varies based on mode
		switch(LR_r.get_mode()){
			case(LoadRegulator::CC):
				Serial.send_string("CC");
				Serial.send_string(", ", LR_measured_voltage, 3, " V");
				Serial.send_string(", ", LR_measured_current, 3, " A");
				Serial.send_string(" / ", LR_r.get_target_current(), 3, " A");
				Serial.send_string(" (", LR_control_current, 3, " A)");
				break;
			case(LoadRegulator::CP):
				Serial.send_string("CP");
				Serial.send_string(", ", LR_measured_voltage, 3, " V");
				Serial.send_string(", ", LR_measured_voltage * LR_measured_current, 3, " W");
				Serial.send_string(" / ", LR_r.get_target_power(), 3, " W");
				Serial.send_string(" (", LR_control_current, 3, " A)");
				break;
			case(LoadRegulator::CR):
				Serial.send_string("CR");
				Serial.send_string(", ", LR_measured_voltage, 3, " V");
				Serial.send_string(", ", LR_measured_voltage / LR_measured_current, 3, " R");
				Serial.send_string(" / ", LR_r.get_target_resistance(), 3, " R");
				Serial.send_string(" (", LR_control_current, 3, " A)");
				break;
			case(LoadRegulator::CV):
				Serial.send_string("CV");
				Serial.send_string(", ", LR_measured_voltage, 3, " V");
				Serial.send_string(" / ", LR_r.get_target_voltage(), 3, " V");
				Serial.send_string(", ", LR_measured_current, 3, " A");
				Serial.send_string(" (", LR_control_current, 3, " A)");
				break;
			case(LoadRegulator::OFF):
				Serial.send_string("OFF");
				Serial.send_string(", ", LR_measured_voltage, 3, "");
				break;
		}
		Serial.send_string(" | ", TR_r.get_duty_cycle(), "%\r\n");
		LED_G.off();
	
		// Check input
		if(Serial.read_rx_buffer(char_buffer, SET_UART_CHAR_BUFFER_SIZE)){
			if(char_buffer[0] == 'c')
				// Put LR into constant current mode
				LR_r.set_mode(LoadRegulator::CC);
			else if(char_buffer[0] == 'p')
				// Put LR into constant power mode
				LR_r.set_mode(LoadRegulator::CP);
			else if(char_buffer[0] == 'r')
				// Put LR into constant resistance mode
				LR_r.set_mode(LoadRegulator::CR);
			else if(char_buffer[0] == 'v')
				// Put LR into constant voltage mode
				LR_r.set_mode(LoadRegulator::CV);
			else if(char_buffer[0] == 'o')
				// Put LR into OFF mode
				LR_r.set_mode(LoadRegulator::OFF);
			else if((char_buffer[0] >= 48) && (char_buffer[0] <= 57))
				// Input is number; adjust set current, power or resistance
				switch(LR_r.get_mode()){
					case(LoadRegulator::CC):
						LR_r.set_target_current(atof(char_buffer));
						break;
					case(LoadRegulator::CP):
						LR_r.set_target_power(atof(char_buffer));
						break;
					case(LoadRegulator::CR):
						LR_r.set_target_resistance(atof(char_buffer));
						break;
					case(LoadRegulator::CV):
						LR_r.set_target_voltage(atof(char_buffer));
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
	}
}