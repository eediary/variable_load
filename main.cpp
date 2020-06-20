/*
 * LoadRegulator.cpp
 *
 * Created: 5/7/2020 10:04:19 PM
 * Author : jaekyung
 */ 

// Project headers
#include "schematic.h"
#include "settings.h"
// AVR headers
#include <avr/interrupt.h>
#include <util/delay.h>
// HAL headers
#include "HAL_Timer.h"
// Module headers
#include "LoadRegulator.h"
#include "TempRegulator.h"
#include "Debugger.h"
#include "User_Interface.h"

int main(void)
{
	// Main Timer constructor
	HAL_Timer MainTimer = HAL_Timer(SET_MAIN_TIMER_NUMBER, SET_MAIN_TIMER_DIV, SET_MAIN_TIMER_TOP);
	MainTimer.enable_int();
	
	// Load Regulator constructor
	LoadRegulator LoadReg;
	
	// Temperature Regulator constructor
	TempRegulator TempReg(MainTimer);
	
	// state variable
	LoadRegulator::LR_state LR_state_var;
	LoadReg.get_state(LR_state_var);
	TempRegulator::TR_state TR_state_var;
	TempReg.get_state(TR_state_var);
	
	// User Interface constructor
	User_Interface UI(LR_state_var, TR_state_var, MainTimer);
	
	// Debugger constructor
	#if DEBUG == 1
	Debugger DebuggerModule = Debugger(LoadReg, TempReg, MainTimer);
	#endif
	
	// Recal zero after delay
	_delay_ms(SET_CAL_DELAY);
	LoadReg.calibrate_zero();
	
	// Enable global interrupt
	sei();
    while (1) 
    {
		// Run load regulator and update state
		LoadReg.regulate();
		LoadReg.get_state(LR_state_var);
		
		// Run temp regulator and update state
		TempReg.regulate();
		TempReg.get_state(TR_state_var);
		
		// Run user interface
		UI.update_screen();
		
		// Update regulators
		LoadReg.set_state(LR_state_var);
		TempReg.set_state(TR_state_var);
		
		// Run debugger
		#if DEBUG == 1
		DebuggerModule.run_debugger();
		#endif
		
    }
}

