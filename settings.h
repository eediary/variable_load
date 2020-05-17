#pragma once

/********************* LOAD REGULATOR *********************/
// Load Regulator sampling frequency
// Set to 1 ms
#define SET_LR_TIMER_NUMBER (HAL_Timer::TIMER_TIMER1)
#define SET_LR_TIMER_DIV (HAL_Timer::TIMER_CLK_DIV64)
#define SET_LR_TIMER_TOP (249)
// Current control parameters
#define SET_LR_CUR_ERROR_SCALER (0.5)
// Zero current calibration amount
#define SET_LR_CAL_AMOUNT 10
