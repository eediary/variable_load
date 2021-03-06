#pragma once
#include "schematic.h"

/********************* MAIN *********************/
// Set to 1 ms
#define SET_MAIN_TIMER_NUMBER (HAL_Timer::TIMER_TIMER3)
#define SET_MAIN_TIMER_DIV (HAL_Timer::TIMER_CLK_DIV64)
#define SET_MAIN_TIMER_TOP (249)
// Start-up delay
#define SET_CAL_DELAY (10) // in ms
// Temperature fail-safe
#define SET_MAX_TEMP (75)
// Power down mode
#define SET_SLEEP_MODE (SLEEP_MODE_IDLE)

/********************* LOAD REGULATOR *********************/
// Load Regulator sampling frequency
// Set to 100 us (10 kHz)
#define SET_LR_TIMER_NUMBER (HAL_Timer::TIMER_TIMER1)
#define SET_LR_TIMER_DIV (HAL_Timer::TIMER_CLK_DIV64)
#define SET_LR_TIMER_TOP (24)
// periods
#define SET_LR_DESIRED_UPDATE_PERIOD (10) // time between target updates, in units of LR timer
#define SET_LR_OFFSET_UPDATE_PERIOD (167) // time between offset updates, in units of LR timer
#define SET_LR_CAL_DELAY_PERIOD (100) // delay time for calibration, in units of LR timer
// offset limits
#define SET_LR_OFFSET_MIN (-0.05) // offset minimum, in amps
#define SET_LR_OFFSET_MAX (0.05) // offset maximum, in amps
// Current control parameters
#define SET_LR_CUR_ERROR_SCALER (0.5)
// Zero current calibration amount
#define SET_LR_CAL_AMOUNT (10) // number of samples to average
#define SET_LR_CAL_FLUSH_AMOUNT (5) // number of samples to discard
// Target max values
#define SET_LR_CUR_MAX (30) // in amps
#define SET_LR_POW_MAX (400) // in watts
#define SET_LR_RES_MAX (999.99) // in ohms
#define SET_LR_VOLT_MAX (30) // in volts
// Desired current limits
#define SET_LR_DESIRED_CUR_MAX (SET_LR_CUR_MAX)
#define SET_LR_DESIRED_CUR_MIN (0)
// Initial target values
#define SET_LR_INIT_CUR (0) // in amps
#define SET_LR_INIT_POW (0) // in watts
#define SET_LR_INIT_RES (SET_LR_RES_MAX) // in ohms
#define SET_LR_INIT_V (SET_LR_VOLT_MAX) // in volts
// CV current step size
#define SET_LR_CV_CUR_STEP (0.001) // in amps
// CV tolerance
#define SET_LR_CV_TOL (0.01) // in volts
// Current sampling period
#define SET_LR_CUR_SAMPLE_PERIOD (17) // in unites of LR timer
// Sample size for averaging
#define SET_LR_MEAS_CURR_SAMPLES (10) // samples of measured current to average
#define SET_LR_MEAS_VOLT_SAMPLES (20) // samples of measured voltage to average

/********************* Debugger *********************/
// Define DEBUG as 1 to enable debugger
#define DEBUG 1
// Debugger output period
#define SET_DEBUGGER_PERIOD (1000) // in unites of main timer
// UART settings
#define SET_UART_DATA_BITS (HAL_UART::UART_8_BITS)
#define SET_UART_PARITY (HAL_UART::UART_PARITY_NONE)
#define SET_UART_STOP_BITS (HAL_UART::UART_STOP_1)
// Buffer size
#define SET_UART_RX_BUFFER_SIZE (20)
#define SET_UART_TX_BUFFER_SIZE (100)
#define SET_UART_TEMP_BUFFER_SIZE (10)
// Formatting
#define SET_DEBUGGER_FP_WIDTH (7)
#define SET_DEBUGGER_FP_DECIMALS (2)
// Baud rate
#define SET_DEBUGGER_BAUD (57600)
// LED assignment
#define SET_LED_DB_PORT (SCH_LED_BLUE_PORT)
#define SET_LED_DB_PIN  (SCH_LED_BLUE_PIN)
#define SET_LED_DB_ACTIVE (SCH_LED_BLUE_ACTIVE)
#define SET_LED_LR_PORT (SCH_LED_GREEN_PORT)
#define SET_LED_LR_PIN  (SCH_LED_GREEN_PIN)
#define SET_LED_LR_ACTIVE (SCH_LED_GREEN_ACTIVE)
#define SET_LED_TR_PORT (SCH_LED_YELLOW_PORT)
#define SET_LED_TR_PIN  (SCH_LED_YELLOW_PIN)
#define SET_LED_TR_ACTIVE (SCH_LED_YELLOW_ACTIVE)
#define SET_LED_UI_PORT (SCH_LED_RED_PORT)
#define SET_LED_UI_PIN  (SCH_LED_RED_PIN)
#define SET_LED_UI_ACTIVE (SCH_LED_RED_ACTIVE)

/********************* Temp Regulator *********************/
// Temp regulator period
#define SET_TR_PERIOD (500) // in units of main timer
#define SET_TR_ADC_ADJ (HAL_ADC::ADC_RADJUST)
#define SET_TR_TEMP_MIN (35)
#define SET_TR_TEMP_MAX (65)
#define SET_TR_DUTY_CYCLE_MIN (0)
#define SET_TR_DUTY_CYCLE_MAX (100)
#define SET_TR_TEMP_TO_DUTY_CYCLE(T) ((SET_TR_DUTY_CYCLE_MAX-SET_TR_DUTY_CYCLE_MIN)/(SET_TR_TEMP_MAX-SET_TR_TEMP_MIN)*(T-SET_TR_TEMP_MIN)+SET_TR_DUTY_CYCLE_MIN)

/********************* User Interface *********************/
// Encoder
#define SET_UI_ENC_LONG_PRESS_THRESH (600) // time necessary to be considered long press, in units of main timer
// LCD
#define SET_UI_LCD_UPDATE_PERIOD (1000) // time between full screen updates, in units of main timer
#define SET_UI_LCD_CLEAR_PERIOD (2) // time needed for clearing screen, in units of main timer
// Screen
#define SET_UI_SCREEN_MAX_LINES (8)
// Formatting
	// formatting for measured values
#define SET_UI_MEASURED_VOLT_WIDTH 6
#define SET_UI_MEASURED_VOLT_DECIMAL 2
#define SET_UI_MEASURED_CURR_WIDTH 6
#define SET_UI_MEASURED_CURR_DECIMAL 2
#define SET_UI_MEASURED_POW_WIDTH 6
#define SET_UI_MEASURED_POW_DECIMAL 2
#define SET_UI_MEASURED_RES_WIDTH 3
#define SET_UI_MEASURED_RES_DECIMAL 0
	// formatting for target values
#define SET_UI_TARGET_VOLT_WIDTH 6
#define SET_UI_TARGET_VOLT_DECIMAL 2
#define SET_UI_TARGET_CURR_WIDTH 6
#define SET_UI_TARGET_CURR_DECIMAL 2
#define SET_UI_TARGET_POW_WIDTH 6
#define SET_UI_TARGET_POW_DECIMAL 2
#define SET_UI_TARGET_RES_WIDTH 3
#define SET_UI_TARGET_RES_DECIMAL 0
	// formatting for local values
#define SET_UI_LOCAL_TARGET_CUR_WIDTH 5
#define SET_UI_LOCAL_TARGET_CUR_DECIMAL 2
#define SET_UI_LOCAL_TARGET_POW_WIDTH 6
#define SET_UI_LOCAL_TARGET_POW_DECIMAL 2
#define SET_UI_LOCAL_TARGET_RES_WIDTH 6
#define SET_UI_LOCAL_TARGET_RES_DECIMAL 2
#define SET_UI_LOCAL_TARGET_VOLT_WIDTH 5
#define SET_UI_LOCAL_TARGET_VOLT_DECIMAL 2
	// formatting for temp values
#define SET_UI_TEMP_WIDTH 3
#define SET_UI_TEMP_DECIMAL 0