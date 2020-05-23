#pragma once

/********************* MAIN *********************/
#define SET_MAIN_TIMER_NUMBER (HAL_Timer::TIMER_TIMER3)
#define SET_MAIN_TIMER_DIV (HAL_Timer::TIMER_CLK_DIV1024)
#define SET_MAIN_TIMER_TOP (15624)

/********************* LOAD REGULATOR *********************/
// Load Regulator sampling frequency
// Set to 1 ms
#define SET_LR_TIMER_NUMBER (HAL_Timer::TIMER_TIMER1)
#define SET_LR_TIMER_DIV (HAL_Timer::TIMER_CLK_DIV64)
#define SET_LR_TIMER_TOP (249)
// Current control parameters
#define SET_LR_CUR_ERROR_SCALER (0.5)
// Zero current calibration amount
#define SET_LR_CAL_AMOUNT (10)
// Initial resistance
#define SET_LR_INIT_RES (1000)

/********************* Debugger *********************/
// Define DEBUG as 1 to enable debugger
#define DEBUG 1
// UART settings
#define SET_UART_DATA_BITS (HAL_UART::UART_8_BITS)
#define SET_UART_PARITY (HAL_UART::UART_PARITY_NONE)
#define SET_UART_STOP_BITS (HAL_UART::UART_STOP_1)
#define SET_UART_CHAR_BUFFER_SIZE (20)
// Baud rate
#define SET_DEBUGGER_BAUD (57600)

/********************* Temp Regulator *********************/
#define SET_TR_ADC_ADJ (HAL_ADC::ADC_RADJUST)