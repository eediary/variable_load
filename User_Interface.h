#pragma once
// Project headers
#include "schematic.h"
#include "settings.h"
// HAL headers
#include "HAL_Timer.h"
// Device headers
#include "Encoder.h"
#include "LiquidCrystal_PCF8574.h"
// Software headers
#include "Screen.h"

class User_Interface{
private:
// Class members
HAL_Timer &Timer;
Encoder Enc;
LiquidCrystal_PCF8574 Lcd;
Main_Screen Main_screen;
Menu_Screen Menu_screen;
Test_Screen Test_screen;
// Data members
int cur_col, cur_row; // col and row to print
long last_update; // last time screen was updated
bool print_flag; // flag to print
Screen *cur_screen;

public:
User_Interface(HAL_Timer &Timer_r);
void update_screen();
};