#pragma once
// Standard headers
#include <string.h>
// Project headers
#include "schematic.h"
#include "settings.h"
// Module headers
#include "LoadRegulator.h"
#include "TempRegulator.h"
// Device headers
#include "LED.h"
// Standard headers
#include <stdlib.h>
// HAL headers
#include "HAL_UART.h"
#include "HAL_Timer.h"

class Debugger{
private:
char char_rx_buffer[SET_UART_RX_BUFFER_SIZE];
char char_tx_buffer[SET_UART_TX_BUFFER_SIZE];

// Class members
LoadRegulator &LR_r;
TempRegulator &TR_r;
HAL_Timer &Timer;
HAL_UART Serial;
LED LED_G;
LED LED_B;
LED LED_Y;
LED LED_R;
// Data members
unsigned long last_time; // last time called, in ms
// Methods
void append_float(float val);
void append_duty_cycle();

public:
Debugger(LoadRegulator &LoadRegulator_r, TempRegulator &TempRegulator_r, HAL_Timer &Timer_r);
void run_debugger();
};