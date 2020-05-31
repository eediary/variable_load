#pragma once
// Project headers
#include "schematic.h"
#include "settings.h"
// Device headers
#include "Encoder.h"
#include "LiquidCrystal_PCF8574.h"
// Standard libraries
#include <string.h>

// Special characters & lines
#define BLANK_LINE "                    "
#define CURSOR_ICON (0x7E)

// Screen sizes
#define MAIN_SIZE (3)
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
private:
	int row_offset;
	int cursor_row;
	void update_row_offset();
protected:
	char text[SET_UI_SCREEN_MAX_LINES][SCH_UI_LCD_COLS + 1];
	bool show_cursor;
	uint8_t number_of_rows; // number of rows for this screen
public:
	Screen();
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
	char get_char(int col, int row);
	void increment_cursor();
	void decrement_cursor();
	void reset_cursor();
	int get_cursor();
};

/********************* Main screen *********************/
class Main_Screen : public Screen{
	public:
	Main_Screen();
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};

/********************* Menu screen *********************/
class Menu_Screen : public Screen{
public:
	Menu_Screen();
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};

/********************* Test screen *********************/
class Test_Screen : public Screen{
	public:
	Test_Screen();
	virtual SCREEN_ID handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn);
};