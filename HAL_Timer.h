#pragma once
#include <avr/io.h>
/*
Uses Timer/Counter 1 or 3 for timer
Equation for frequency of interrupt occurrences is:
    F = F_IO / (N * (1 + OCRNA))
F_IO is frequency of IO clock
N is prescaler / divider (1, 8, 64, 256 or 1024)
OCRNA is value in OCRNA register (0 ~ 255)
At 16 MHz F_IO, interrupt frequency is 0.238 Hz to 16 MHz
    T = N / F_IO * (1 + OCRNA)
    OCRNA = T * F_IO / N - 1
T is time between interrupt occurrences
Equations above are rewritten forms of first equation
*/

class HAL_Timer{
public:
	// ATMEGA32U4 has two counter modules, TIMER1 and TIMER3
	enum Timer_module{
		TIMER_TIMER1, TIMER_TIMER3
	};
	enum Timer_clk_sel{
		TIMER_NO_CLK = 0b000,
		TIMER_CLK_DIV1 = 0b001, TIMER_CLK_DIV8 = 0b010, TIMER_CLK_DIV64 = 0b011, 
		TIMER_CLK_DIV256 = 0b100, TIMER_CLK_DIV1024 = 0b101,
		TIMER_EXT_CLK_FALLING = 0b110, TIMER_EXT_CLK_RISING = 0b111
	};
	enum Timer_op_mode{
		TIMER_CTC_OCRNA = 0b0100
	};
private:
	volatile uint8_t *TCCRNA, *TCCRNB, *TCCRNC;
	volatile uint16_t *TCNTN;
	volatile uint16_t *OCRNA;
	volatile uint8_t *TIMSKN;
	volatile uint8_t *TIFRN;
public:
	// constructor
	// constructor
HAL_Timer(Timer_module mod_number, Timer_clk_sel clk_src, uint16_t top){
	// set up pointers
	if(mod_number == TIMER_TIMER1){
		TCCRNA = &TCCR1A;
		TCCRNB = &TCCR1B;
		TCCRNC = &TCCR1C;
		TCNTN = &TCNT1;
		OCRNA = &OCR1A;
		TIMSKN = &TIMSK1;
		TIFRN = &TIFR1;
	}else if(mod_number == TIMER_TIMER3){
		TCCRNA = &TCCR3A;
		TCCRNB = &TCCR3B;
		TCCRNC = &TCCR3C;
		TCNTN = &TCNT3;
		OCRNA = &OCR3A;
		TIMSKN = &TIMSK3;
		TIFRN = &TIFR3;
	}
	// disable & reset counter
	disable_timer();
	clear_counter();
	
	// set top
	set_top(top);
	
	// Use CTC, with OCRNA as top
	*TCCRNA = (*TCCRNA & ~(0b11 << WGM10)) | ((TIMER_CTC_OCRNA & 0b0011) << WGM10);
	*TCCRNB = (*TCCRNB & ~(0b11 << WGM12)) | (((TIMER_CTC_OCRNA & 0b1100) >> 2) << WGM12);
	
	// enable counter
	enable_timer(clk_src);
}
// enable / disable timer
void enable_timer(Timer_clk_sel clk_src){
	*TCCRNB = (*TCCRNB & ~(0b111 << CS10)) | (clk_src << CS10);
}
void disable_timer(){
	*TCCRNB &= ~(0b111 << CS10);
}
// top and counter manipulation
uint16_t get_top(){
	return *OCRNA;
}
void set_top(uint16_t val){
	*OCRNA = val;
}
uint16_t get_counter(){
	return *TCNTN;
}
void set_counter(uint16_t val){
	*TCNTN = val;
}
void clear_counter(){
	set_counter(0);
}
// interrupts
void enable_int(){
	*TIMSKN |= (0b1 << OCIE1A);
}
void disable_int(){
	*TIMSKN &= ~(0b1 << OCIE1A);
}
bool get_int_flag(){
	return (*TIFRN & (0b1 << OCF1A));
}
void clear_int_flag(){
	*TIFRN |= (0b1 << OCF1A);
}
};

