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
// Module headers
#include "LoadRegulator.h"
#include "TempRegulator.h"

class User_Interface{
private:
// State members
LoadRegulator::LR_state &_LR_state;
TempRegulator::TR_state &_TR_state;
// Class members
HAL_Timer &Timer;
Encoder Enc;
LiquidCrystal_PCF8574 Lcd;
// Screens
VL_Screen VL_screen;
Main_Menu_Screen Main_menu_screen;
LR_Mode_Screen LR_Mode_screen;
LR_Val_Screen LR_Val_screen;
// Data members
int cur_col, cur_row; // col and row to print
long last_update; // last time screen was updated
bool wait_for_clear_flag;
bool print_flag; // flag to print
Screen *cur_screen; // pointer to current screen
char screen_chars[SCH_UI_LCD_ROWS][SCH_UI_LCD_COLS+1]; // characters to print

public:
User_Interface(LoadRegulator::LR_state &LR_state_r, TempRegulator::TR_state &TR_state_r, HAL_Timer &Timer_r);
void update_screen();
};