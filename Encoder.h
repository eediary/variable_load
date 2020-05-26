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
	static volatile Encoder_Gray last_state;
	static volatile Encoder_Dir dir;

private:	
	HAL_GPIO &A;
	HAL_GPIO &B;
	
public:
	Encoder(HAL_GPIO &A_r, HAL_GPIO &B_r) :
	A(A_r), B(B_r)
	{
		// Constructor
		// Set up static public members
		A_Ptr = &A;
		B_Ptr = &B;
		last_state = ONE_ONE;
		dir = NONE;
		
		// Configure A and B
		A.set_direction(HAL_GPIO::GPIO_INPUT);
		B.set_direction(HAL_GPIO::GPIO_INPUT);
		A.gpio_interrupt_enable();
		B.gpio_interrupt_enable();
	}
	
	Encoder_Dir get_dir(){
		// reading dir clears it
		Encoder_Dir to_return = dir;
		dir = NONE;
		return to_return;
	}
};

