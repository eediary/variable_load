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
#include <avr/sleep.h>
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
	
	// User Interface constructor
	User_Interface UI(LoadReg, TempReg, MainTimer);
	
	// Debugger constructor
	#if DEBUG == 1
	Debugger DebuggerModule = Debugger(LoadReg, TempReg, MainTimer);
	#endif
	
	// Recal zero after delay
	_delay_ms(SET_CAL_DELAY);
	LoadReg.calibrate_zero();
	
	// Configure sleep mode
	set_sleep_mode(SET_SLEEP_MODE);
	
	// Enable global interrupt
	sei();
    while (1) 
    {
		// Run load regulator and update state
		LoadReg.regulate();
		
		// Run temp regulator and update state
		TempReg.regulate();
		
		// Run user interface
		UI.update_screen();
		
		// Fail-safe
		if(TempReg.get_temp() >= SET_MAX_TEMP){
			// Load is too hot; disable load regulator
			LoadReg.set_mode(LoadRegulator::OFF);
		}
		
		// Run debugger
		#if DEBUG == 1
		DebuggerModule.run_debugger();
		#endif
		
		// Sleep until interrupt occurs
		cli();
		sleep_enable();
		sei();
		sleep_cpu();
		// Interrupt has occurred; wake up CPU
		sleep_disable();
		sei();
		
		
    }
}

