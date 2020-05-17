#pragma once

/********************* LOAD REGULATOR *********************/
// Load Regulator sampling frequency
// Set to 1 ms
#define SET_LR_TIMER_NUMBER (HAL_Timer::TIMER_TIMER1)
#define SET_LR_TIMER_DIV (HAL_Timer::TIMER_CLK_DIV64)
#define SET_LR_TIMER_TOP (249)
// Current step size
// set to 1 mA
#define SET_LR_CUR_STEP_SIZE (0.001)
// Zero current calibration amount
#define SET_LR_CAL_AMOUNT 10
