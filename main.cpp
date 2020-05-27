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
	// Main Timer
	HAL_Timer MainTimer = HAL_Timer(SET_MAIN_TIMER_NUMBER, SET_MAIN_TIMER_DIV, SET_MAIN_TIMER_TOP);
	MainTimer.enable_int();
	
	// Load Regulator
	LoadRegulator LoadRegulatorClass;
	
	// Temperature Regulator
	TempRegulator TempRegulatorClass;
	
	// Debugger
	#if DEBUG == 1
	Debugger DebuggerModule = Debugger(SET_DEBUGGER_BAUD, LoadRegulatorClass, TempRegulatorClass);
	#endif
	
	// Enable global interrupt
	sei();
    while (1) 
    {
		LoadRegulatorClass.regulate();
		
		if(MainTimer.get_flag()){
			MainTimer.clear_flag();
			// Run regulator
			TempRegulatorClass.regulate();
			
			// Debugger
			#if DEBUG == 1
			DebuggerModule.run_debugger();
			#endif
		}
		
    }
}

