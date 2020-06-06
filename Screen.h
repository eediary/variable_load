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

// Special characters & lines
#define BLANK_LINE "                    "
#define CURSOR_ICON (0x7E)

// Screen sizes
#define MAIN_SIZE (8)
#define MENU_SIZE (5)
#define TEST_SIZE (1)

/********************* Screen class *********************/
class Screen{
public:
	enum SCREEN_ID{
		MAIN_SCREEN = 0,
		MENU_SCREEN = 1,
		TEST_SCREEN = 2
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

/********************* Main screen *********************/
class Main_Screen : public Screen{
private:
	virtual void update_text();
public:
	Main_Screen();
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};

// /********************* Menu screen *********************/
// class Menu_Screen : public Screen{
// public:
// 	Menu_Screen();
// 	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
// };
// 
// /********************* Test screen *********************/
// class Test_Screen : public Screen{
// 	public:
// 	Test_Screen();
// 	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
// };