#pragma once
#include "schematic.h"

/********************* MAIN *********************/
// Set to 1 ms
#define SET_MAIN_TIMER_NUMBER (HAL_Timer::TIMER_TIMER1)
#define SET_MAIN_TIMER_DIV (HAL_Timer::TIMER_CLK_DIV64)
#define SET_MAIN_TIMER_TOP (249)

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
// CR initial resistance
#define SET_LR_INIT_RES (1000) // in ohms
// CV current step size
#define SET_LR_CV_CUR_STEP (0.001) // in mA
// CV initial voltage
#define SET_LR_CV_INIT_V (100) // in V
// Current sampling period
#define SET_LR_CUR_SAMPLE_PERIOD (17) // in ms

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
#define SET_TR_TEMP_MIN (SCH_THERM_35C_VOLT)
#define SET_TR_TEMP_MAX (SCH_THERM_85C_VOLT)
#define SET_TR_DUTY_CYCLE_MIN (0)
#define SET_TR_DUTY_CYCLE_MAX (100)
#define SET_TR_VOLT_TO_DUTY_CYCLE(V) ((SET_TR_DUTY_CYCLE_MAX-SET_TR_DUTY_CYCLE_MIN)/(SET_TR_TEMP_MAX-SET_TR_TEMP_MIN)*(V-SET_TR_TEMP_MIN)+SET_TR_DUTY_CYCLE_MIN)

/********************* User Interface *********************/
// Encoder
#define SET_UI_ENC_LONG_PRESS_THRESH (1000) // time necessary to be considered long press, in ms
// LCD
#define SET_UI_LCD_UPDATE_PERIOD (200) // time between full screen updates, in ms
// Screen
#define SET_UI_SCREEN_MAX_LINES (8)