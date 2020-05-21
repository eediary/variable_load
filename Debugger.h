#pragma once
// Project headers
#include "schematic.h"
#include "settings.h"
// Module headers
#include "LoadRegulator.h"
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
HAL_UART Serial;
LoadRegulator &LR_r;
LED LED_G;
LED LED_B;
LED LED_Y;
LED LED_R;

public:
Debugger(long baud_rate, LoadRegulator &LoadRegulator_r);
void run_debugger();
};