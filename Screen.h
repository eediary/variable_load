#pragma once
// Project headers
#include "schematic.h"
#include "settings.h"
// Device headers
#include "Encoder.h"
#include "LiquidCrystal_PCF8574.h"
// Standard libraries
#include <stdlib.h>
#include <string.h>
// Module headers
#include "LoadRegulator.h"
#include "TempRegulator.h"

// Special characters & lines
#define BLANK_LINE "                    "
#define CURSOR_ICON (0x7E)

// Screen sizes
#define VL_SIZE (4)
#define MAIN_MENU_SIZE (5)
#define LR_MODE_SIZE (6)
#define LR_VAL_SIZE (2)
#define TR_VAL_SIZE (2)

/********************* Screen class *********************/
class Screen{
public:
	enum SCREEN_ID{
		VL_SCREEN,
		MAIN_MENU_SCREEN,
		LR_MODE_SCREEN,
		LR_VAL_SCREEN,
		TR_VAL_SCREEN
	};
protected:
	int row_offset;
	int cursor_row;
	int cursor_row_min;
	bool show_cursor;
	uint8_t number_of_rows; // number of rows for this screen
	
	void update_row_offset();
	char text[SET_UI_SCREEN_MAX_LINES][SCH_UI_LCD_COLS + 2]; // all text for screen
	void limit_cursor();
	void increment_cursor();
	void decrement_cursor();
	void reset_cursor();
	int get_cursor();
	virtual void update_text(); // updates all text
public:
	Screen();
	void update_screen_chars(char screen_chars[SCH_UI_LCD_ROWS][SCH_UI_LCD_COLS+1]); // loads text to print
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};

/********************* VL screen *********************/
class VL_Screen : public Screen{
	private:
		virtual void update_text();
		LoadRegulator::LR_state &_LR_state;
		TempRegulator::TR_state &_TR_state;
	public:
		VL_Screen(LoadRegulator::LR_state &LR_state_r, TempRegulator::TR_state &TR_state_r);
		virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};

/********************* Main Menu screen *********************/
class Main_Menu_Screen : public Screen{
private:
	virtual void update_text();
public:
	Main_Menu_Screen();
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};

/********************* LR Mode screen *********************/
class LR_Mode_Screen : public Screen{
private:
	virtual void update_text();
	LoadRegulator::LR_state &_LR_state;
public:
	LR_Mode_Screen(LoadRegulator::LR_state &LR_state_r);
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};
/********************* LR Val screen *********************/
class LR_Val_Screen : public Screen{
private:
	bool update_local_val; // flag for copying target val to local variable
	float local_target_val; // local variable that will be copied to target val
	virtual void update_text();
	LoadRegulator::LR_state &_LR_state;
public:
	LR_Val_Screen(LoadRegulator::LR_state &LR_state_r);
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};
/********************* TR Val screen *********************/
class TR_Val_Screen : public Screen{
private:
	int index; // used to determine duty cycle
	int index_to_duty_cycle();
	virtual void update_text();
	TempRegulator::TR_state &_TR_state;
public:
	TR_Val_Screen(TempRegulator::TR_state &TR_state_r);
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};