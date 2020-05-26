#include "HAL_Timer.h"

volatile unsigned long HAL_Timer::timer1_tick = 0;
volatile unsigned long HAL_Timer::timer3_tick = 0;
volatile bool HAL_Timer::timer1_flag = false;
volatile bool HAL_Timer::timer3_flag = false;

ISR(TIMER1_COMPA_vect){
	HAL_Timer::timer1_flag = true;
	HAL_Timer::timer1_tick++;
}
ISR(TIMER3_COMPA_vect){
	HAL_Timer::timer3_flag = true;
	HAL_Timer::timer3_tick++;
}