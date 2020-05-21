#include "Debugger.h"

Debugger::Debugger(long baud_rate, LoadRegulator &LoadRegulator_r):
	Serial(baud_rate, SCH_UART_MODE, SET_UART_DATA_BITS, SET_UART_PARITY, SET_UART_STOP_BITS),
	LR_r(LoadRegulator_r),
	LED_G(SCH_LED_GREEN_PORT, SCH_LED_GREEN_PIN, SCH_LED_GREEN_ACTIVE),
	LED_B(SCH_LED_BLUE_PORT, SCH_LED_BLUE_PIN, SCH_LED_BLUE_ACTIVE),
	LED_Y(SCH_LED_YELLOW_PORT, SCH_LED_YELLOW_PIN, SCH_LED_YELLOW_ACTIVE),
	LED_R(SCH_LED_RED_PORT, SCH_LED_RED_PIN, SCH_LED_RED_ACTIVE)
{
	Serial.enable_rx_int();
	Serial.send_string("Debugger running...\r\n");
}

void Debugger::run_debugger(){
	LED_G.on();
	Serial.send_string("", LR_r.get_mode(), ", ");
	Serial.send_string("", LR_r.get_measured_voltage(), 3, " V, ");
	Serial.send_string("", LR_r.get_measured_current(), 3, " A / ");
	Serial.send_string("", LR_r.get_control_current(), 3, " A / ");
	Serial.send_string("", LR_r.get_target_current(), 3, " A\r\n");
	LED_G.off();
	
	// Check input
	if(Serial.read_rx_buffer(char_buffer, SET_UART_CHAR_BUFFER_SIZE)){
		if(char_buffer[0] == 'c')
		// Put LR into constant current mode
		LR_r.set_mode(LoadRegulator::CC);
		else if(char_buffer[0] == 'o')
		// Put LR into OFF mode
		LR_r.set_mode(LoadRegulator::OFF);
		else
		// Input is number; adjust set current
		LR_r.set_target_current(atof(char_buffer));
	}
}