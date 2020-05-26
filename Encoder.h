#pragma once

#include <stddef.h>
#include <avr/interrupt.h>

#include "HAL_GPIO.h"

class Encoder{
public:
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

	// static public members are shared with ISR
	static HAL_GPIO *A_Ptr;
	static HAL_GPIO *B_Ptr;
	static HAL_GPIO *BTN_Ptr;
	static volatile Encoder_Gray last_state;
	static volatile Encoder_Dir dir;
	static volatile bool pressed;

private:	
	HAL_GPIO &A;
	HAL_GPIO &B;
	HAL_GPIO &BTN;
	
public:
	Encoder(HAL_GPIO &A_r, HAL_GPIO &B_r, HAL_GPIO &BTN_r) :
	A(A_r), B(B_r), BTN(BTN_r)
	{
		// Constructor
		// Set up static public members
		A_Ptr = &A;
		B_Ptr = &B;
		BTN_Ptr = &BTN;
		last_state = ONE_ONE;
		dir = NONE;
		pressed = false;
		
		// Configure A, B and BTN
		A.set_direction(HAL_GPIO::GPIO_INPUT);
		B.set_direction(HAL_GPIO::GPIO_INPUT);
		BTN.set_direction(HAL_GPIO::GPIO_INPUT);
		A.gpio_interrupt_enable();
		B.gpio_interrupt_enable();
		BTN.gpio_interrupt_enable();
	}
	
	Encoder_Dir get_dir(){
		// reading dir clears it
		Encoder_Dir to_return = dir;
		dir = NONE;
		return to_return;
	}
	
	bool get_pressed(){
		// reading is_pressed clears it
		bool to_return = pressed;
		pressed = false;
		return to_return;
	}
};

