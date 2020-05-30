#pragma once

#include <stddef.h>
#include <avr/interrupt.h>

#include "settings.h"

#include "HAL_GPIO.h"
#include "HAL_Timer.h"

class Encoder{
public:
	enum Encoder_Lead{
		A_LEADS_B = 0,
		B_LEADS_A = 1
	};
	enum Encoder_Dir{
		NONE = 0,
		CLOCKWISE = 1,
		COUNTERCLOCKWISE = 2
	};
	enum Encoder_Gray{
		ONE_ONE = 0b11,
		ONE_ZERO = 0b10,
		ZERO_ONE = 0b01,
		ZERO_ZERO = 0b00
	};
	enum Encoder_Button{
		NO_PUSH = 0,
		PUSH = 1,
		LONG_PUSH = 2
	};

	// static public members are shared with ISR
	static HAL_GPIO *A_Ptr;
	static HAL_GPIO *B_Ptr;
	static HAL_GPIO *BTN_Ptr;
	static HAL_Timer *Timer_Ptr;
	// Encoder variables
	static volatile Encoder_Gray last_state;
	static volatile Encoder_Dir dir;
	// Button variables
	static volatile Encoder_Button pressed;
	static volatile unsigned long button_time;
	static volatile bool check_time;

private:	
	HAL_GPIO &A;
	HAL_GPIO &B;
	HAL_GPIO &BTN;
	HAL_Timer &Timer;
	Encoder_Lead lead;
	
public:
	Encoder(HAL_GPIO &A_r, HAL_GPIO &B_r, HAL_GPIO &BTN_r, HAL_Timer &Timer_r, Encoder_Lead encoder_lead) :
	A(A_r), B(B_r), BTN(BTN_r), Timer(Timer_r), lead(encoder_lead)
	{
		// Constructor
		// Set up static public members
		A_Ptr = &A;
		B_Ptr = &B;
		BTN_Ptr = &BTN;
		Timer_Ptr = &Timer;
		last_state = ONE_ONE;
		dir = NONE;
		pressed = NO_PUSH;
		button_time = 0;
		check_time = false;
		
		// Configure A, B and BTN
		A.set_direction(HAL_GPIO::GPIO_INPUT);
		B.set_direction(HAL_GPIO::GPIO_INPUT);
		BTN.set_direction(HAL_GPIO::GPIO_INPUT);
		A.gpio_interrupt_enable();
		B.gpio_interrupt_enable();
		BTN.gpio_interrupt_enable();
		
		// Configure Timer
		Timer.enable_int();
	}
	
	Encoder_Dir get_dir(){
		// reading dir clears it
		Encoder_Dir to_return = dir;
		// dir is written assuming A leads B
		if(lead == B_LEADS_A){
			// reverse direction
			if(to_return == CLOCKWISE)
				to_return = COUNTERCLOCKWISE;
			else if(to_return == COUNTERCLOCKWISE)
				to_return = CLOCKWISE;
		}
		dir = NONE;
		return to_return;
	}
	
	Encoder_Button get_pressed(){
		// If button hasn't been pushed, then return NO_PUSH
		// If button has been pressed, then see if PUSH or LONG_PUSH
		if(pressed == NO_PUSH)
			return NO_PUSH;
		else{
			// Button has been pushed
			// If time exceeds threshold, then considered long press
			if(check_time){
				if(Timer.get_tick() - button_time > SET_UI_ENC_LONG_PRESS_THRESH){
					// Sufficient time has passed to be considered long press
					pressed = NO_PUSH;
					return LONG_PUSH;
				}
				// Insufficient time has passed, so wait
				return NO_PUSH;
			} else{
				// button released early, so regular push
				pressed = NO_PUSH;
				return PUSH;
			}
		}
	}
};

