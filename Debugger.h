#pragma once
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

class Debugger{
private:
char char_buffer[SET_UART_CHAR_BUFFER_SIZE];

// Class members
LoadRegulator &LR_r;
TempRegulator &TR_r;
HAL_UART Serial;
LED LED_G;
LED LED_B;
LED LED_Y;
LED LED_R;

public:
Debugger(long baud_rate, LoadRegulator &LoadRegulator_r, TempRegulator &TempRegulator_r);
void run_debugger();
};