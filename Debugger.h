#pragma once
// Project headers
#include "schematic.h"
#include "settings.h"
// Module headers
#include "LoadRegulator.h"
// Standard headers
#include <stdlib.h>
// HAL headers
#include "HAL_UART.h"

class Debugger{
private:
HAL_UART Serial;
char char_buffer[SET_UART_CHAR_BUFFER_SIZE];
LoadRegulator &LR_r;

public:
Debugger(long baud_rate, LoadRegulator &LoadRegulator_r);
void run_debugger();
};