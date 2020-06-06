/*
 * LoadRegulator.cpp
 *
 * Created: 5/7/2020 10:04:19 PM
 * Author : jaekyung
 */ 

// AVR headers
#include <avr/interrupt.h>
// Project headers
#include "schematic.h"
#include "settings.h"
// HAL headers
#include "HAL_Timer.h"
// Module headers
#include "LoadRegulator.h"
#include "TempRegulator.h"
#include "Debugger.h"

int main(void)
{		
	// Main Timer constructor
	HAL_Timer MainTimer = HAL_Timer(SET_MAIN_TIMER_NUMBER, SET_MAIN_TIMER_DIV, SET_MAIN_TIMER_TOP);
	MainTimer.enable_int();
	
	// Load Regulator constructor
	LoadRegulator LoadRegulatorClass;
	
	// Temperature Regulator constructor
	TempRegulator TempRegulatorClass(MainTimer);
	
	// Debugger constructor
	#if DEBUG == 1
	Debugger DebuggerModule = Debugger(LoadRegulatorClass, TempRegulatorClass, MainTimer);
	#endif
	
	// state variable
	LoadRegulator::LR_state LR_state_var;
	LoadRegulatorClass.get_state(LR_state_var);
	TempRegulator::TR_state TR_state_var;
	TempRegulatorClass.get_state(TR_state_var);
	
	// Enable global interrupt
	sei();
    while (1) 
    {
		// Run load regulator
		LoadRegulatorClass.regulate();
		
		// Run temp regulator
		TempRegulatorClass.regulate();
		
		// Run debugger
		#if DEBUG == 1
		DebuggerModule.run_debugger();
		#endif
		
    }
}

