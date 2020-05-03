// PWM class, uses 8-bit Timer/Counter0
// Outputs on OC0A
#pragma once

#ifndef F_CPU
# warning "F_CPU not defined for PWM.h"
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

class HAL_PWM{
public:
	// enums
	enum PWM_MODE_BITS{
//		PWM_NORMAL = 0b000,
		PWM_PHASE_0XFF = 0b001,
//		CTC = 0b010,
		PWM_FAST_0XFF = 0b011,
//		PWM_PHASE_OCRA = 0b101,
//		PWM_FAST_OCRA = 0b111
	};
	enum PWM_CLK_SEL{
		PWM_NO_CLK = 0b000,
		PWM_CLK_IO = 0b001,
		PWM_CLK_IO_DIV_8 = 0b010,
		PWM_CLK_IO_DIV_64 = 0b011,
		PWM_CLK_IO_DIV_256 = 0b100,
		PWM_CLK_IO_DIV_1024 = 0b101,
		PWM_EXT_CLK_FALLING = 0b110,
		PWM_EXT_CLK_RISING = 0b111
	};
	enum PWM_OUTPUT_MODE{
		PWM_DISCONNECTED = 0b00,
		PWM_TOGGLE = 0b01,
		PWM_NONINVERTED = 0b10,
		PWM_INVERTED = 0b11
	};
private:
	PWM_CLK_SEL counter_clk_source;
public:
	// methods
HAL_PWM(PWM_MODE_BITS mode, PWM_CLK_SEL clk_source, PWM_OUTPUT_MODE output_mode){
	// constructor
	// Sets Timer/Counter0 clock source
	// Sets Timer/Counter0 to mode
	// Sets output mode on OCR0A, disables OCR0B
	
	// Save clock source
	counter_clk_source = clk_source;
	// disables counter for construction
	disable_counter();
	// set Waveform Generation Mode
	TCCR0B = (TCCR0B & ~(0b1 << WGM02)) | ((mode & 0b100) << WGM02);
	TCCR0A = (TCCR0A & ~(0b11 << WGM00)) | ((mode & 0b011) << WGM00);
	// set output mode OCR0A and OCR0B and set pin direction
	TCCR0A = (TCCR0A  & ~((0b11 << COM0A0) | (0b11 << COM0B0))) | (output_mode << COM0A0);
	if(output_mode != PWM_DISCONNECTED)
		DDRB |= (1 << DDB7);
	// Initial duty cycle is 0
	set_duty_cycle(0);
	
	// enables counter
	enable_counter();
}
int set_duty_cycle(int val){
	// sets duty cycle as close to val as possible
	// returns actual duty cycle
	// val should be between 0 and 100, inclusive
	if(val > 100)
		val = 100;
	else if(val < 0)
		val = 0;
	unsigned long temp = (val * 0xFF) / 100;
	OCR0A = (uint8_t) temp;
	return get_duty_cycle();
}
int get_duty_cycle(){
	unsigned long temp = (OCR0A * 100) / 0xFF;
	return (int) temp;
}
void disable_counter(){
	// disables & resets counter
	TCCR0B &= ~(0b111 << CS00);
	TCNT0 = 0;
}
void enable_counter(){
	TCCR0B &= ~(0b111 << CS00);
	TCCR0B |= (counter_clk_source << CS00);
}
};