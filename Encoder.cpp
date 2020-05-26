#include "Encoder.h"

// initialize static members
// Encoder static members are used to share data with ISR
HAL_GPIO *Encoder::A_Ptr = NULL;
HAL_GPIO *Encoder::B_Ptr = NULL;
HAL_GPIO *Encoder::BTN_Ptr = NULL;
volatile Encoder::Encoder_Gray Encoder::last_state = Encoder::ONE_ONE;
volatile Encoder::Encoder_Dir Encoder::dir = Encoder::NONE;
volatile bool Encoder::pressed = false;

// ISR
ISR(PCINT0_vect){
	// Read A, B and BTN
	bool A_val = Encoder::A_Ptr->read_pin();
	bool B_val = Encoder::B_Ptr->read_pin();
	bool BTN_val = Encoder::BTN_Ptr->read_pin();	
	
	// Only update dir if transitioning away from idle state
	if(Encoder::last_state == Encoder::ONE_ONE){
		// Encoder was idle; accept data
		if(A_val == false){
			// A = 0, B = 1
			Encoder::dir = Encoder::CLOCKWISE;
		}else if(B_val == false){
			// A = 1, B = 0
			Encoder::dir = Encoder::COUNTERCLOCKWISE;
		}
	}
	
	// Update last state
	if((A_val == false) && (B_val == false))
		Encoder::last_state = Encoder::ZERO_ZERO;
	else if((A_val == false) && (B_val == true))
		Encoder::last_state = Encoder::ZERO_ONE;
	else if((A_val == true) && (B_val == false))
		Encoder::last_state = Encoder::ONE_ZERO;
	else if((A_val == true) && (B_val == true))
		Encoder::last_state = Encoder::ONE_ONE;
	
	// UPdate is pressed
	if(BTN_val == false)
		Encoder::pressed = true;
}
