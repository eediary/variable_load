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
#define OHM_ICON (0xF4)
#define DEG_ICON (0xDF)

// Screen sizes
#define VL_SIZE (4)
#define MAIN_MENU_SIZE (5)
#define LR_MODE_SIZE (6)
#define LR_VAL_SIZE (3)
#define TR_VAL_SIZE (2)
#define INFO_SIZE (3)

/********************* Screen class *********************/
class Screen{
public:
	enum SCREEN_ID{
		VL_SCREEN,
		MAIN_MENU_SCREEN,
		LR_MODE_SCREEN,
		LR_VAL_SCREEN,
		TR_VAL_SCREEN,
		INFO_SCREEN
	};
protected:
	int row_offset;
	int cursor_row;
	int cursor_row_min;
	bool show_cursor;
	uint8_t number_of_rows; // number of rows for this screen
	
	void update_row_offset();
	char text[SET_UI_SCREEN_MAX_LINES][SCH_UI_LCD_COLS+1]; // all text for screen
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
#define VL_SCREEN_LINE_0 "00.00 V"
#define VL_SCREEN_LINE_1 "00.00 A"
#define VL_SCREEN_LINE_2 "000.00 W"
#define VL_SCREEN_LINE_3 "OFF             000%"
class VL_Screen : public Screen{
	private:
		void res_to_text(float val, char* dest, int width, int prec);
		virtual void update_text();
		LoadRegulator &_LR_r;
		TempRegulator &_TR_r;
	public:
		VL_Screen(LoadRegulator &LR_ref, TempRegulator &TR_ref);
		virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};

/********************* Main Menu screen *********************/
#define MAIN_MENU_LINE_0 " Back"
#define MAIN_MENU_LINE_1 " Set mode"
#define MAIN_MENU_LINE_2 " Set target"
#define MAIN_MENU_LINE_3 " Fan Control"
#define MAIN_MENU_LINE_4 " Info"
class Main_Menu_Screen : public Screen{
private:
	virtual void update_text();
public:
	Main_Menu_Screen();
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};

/********************* LR Val screen *********************/
#define LR_VAL_LINE_0_A "Select digit:"
#define LR_VAL_LINE_0_B "Modify digit:"
#define LR_VAL_LINE_1 "0000.00 A        END"
#define LR_VAL_LINE_2 (BLANK_LINE)
#define LR_VAL_END_OFFSET (17)
class LR_Val_Screen : public Screen{
private:
	bool update_local_val; // flag for copying target val to local variable
	float local_target_val; // local variable that will be copied to target val
	int digit_index; // indicates which digit is selected
	bool select_digit; // true if selecting digit, false if modifying digit
	int display_width; // number of digits for display
	int display_dec; // number of digits for decimal
	float local_val_max; // max allowed value for local val
	LoadRegulator::operation_mode local_op_mode; // op mode to use when updating
	bool use_local_op_mode; // use local op mode if coming from LR Mode screen
	virtual void update_text();
	LoadRegulator &_LR_r;
public:
	LR_Val_Screen(LoadRegulator &LR_ref);
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
	void update_op_mode(LoadRegulator::operation_mode mode);
	LoadRegulator::operation_mode get_op_mode();
};
/********************* LR Mode screen *********************/
#define LR_MODE_LINE_0 " Cancel"
#define LR_MODE_LINE_1 " CC"
#define LR_MODE_LINE_2 " CP"
#define LR_MODE_LINE_3 " CR"
#define LR_MODE_LINE_4 " CV"
#define LR_MODE_LINE_5 " OFF"
class LR_Mode_Screen : public Screen{
	private:
	LoadRegulator &_LR_r;
	LR_Val_Screen &_LR_Val_Screen;
	virtual void update_text();
	public:
	LR_Mode_Screen(LoadRegulator &LR_ref, LR_Val_Screen &LR_Val_Screen_r);
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};
/********************* TR Val screen *********************/
#define TR_VAL_LINE_0 "Select fan speed"
class TR_Val_Screen : public Screen{
private:
	int index; // used to determine duty cycle
	int index_to_duty_cycle();
	virtual void update_text();
	TempRegulator &_TR_r;
public:
	TR_Val_Screen(TempRegulator &TR_ref);
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};
/********************* Info screen *********************/
#define INFO_LINE_0 "EE Diary"
#define INFO_LINE_1 "Variable load"
#define INFO_LINE_2 "2020"
class Info_Screen : public Screen{
	private:
	virtual void update_text();
	public:
	Info_Screen();
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};