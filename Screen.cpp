#include "Screen.h"
/********************* Screen class *********************/
Screen::Screen(){
	row_offset = 0;
	cursor_row = 0;
	show_cursor = false;
	// number_of_rows = SET_UI_SCREEN_MAX_LINES; // undefined reference to vtable?
}
char Screen::get_char(int col, int row){
	int new_row = row + row_offset;
	// if input is invalid, return blank
	if((new_row >= number_of_rows) || (new_row < 0) || (col >= SCH_UI_LCD_COLS) || (col < 0))
		return ' ';
	else{
		// show cursor if relevant
		if(show_cursor && (col == 0) && (new_row == cursor_row))
			// return cursor
			return CURSOR_ICON;
		else
			// return character
			return text[new_row][col];
	}
}
void Screen::update_row_offset(){
	// update row offset if cursor is off screen
	if(row_offset > cursor_row)
		// row offset is too big
		row_offset = cursor_row;
	else if(row_offset + SCH_UI_LCD_ROWS - 1 < cursor_row)
		// row offset is too small
		row_offset = cursor_row - SCH_UI_LCD_ROWS + 1;
}
void Screen::increment_cursor(){
	cursor_row++;
	if(cursor_row >= number_of_rows)
		cursor_row= number_of_rows - 1;
	update_row_offset();
}
void Screen::decrement_cursor(){
	cursor_row--;
	if(cursor_row < 0)
		cursor_row = 0;
	update_row_offset();
}
void Screen::reset_cursor(){
	cursor_row = 0;
	update_row_offset();
}
int Screen::get_cursor(){
	return cursor_row;
}

/********************* Main screen *********************/
Main_Screen::Main_Screen(){
	show_cursor = true;
	number_of_rows = MAIN_SIZE;
	strcpy(text[0], " Main screen:       ");
	strcpy(text[1], " menu screen        ");
	strcpy(text[2], " test screen        ");
}
Screen::SCREEN_ID Main_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// button resets cursor row
	if(btn == Encoder::PUSH){
		if(get_cursor() == 1)
			return Screen::MENU_SCREEN;
		else if(get_cursor() == 2)
			return Screen::TEST_SCREEN;
	}
	
	// dir increments or decrements cursor row
	if(dir == Encoder::CLOCKWISE)
		increment_cursor();
	else if(dir == Encoder::COUNTERCLOCKWISE)
		decrement_cursor();
	
	// don't change screens
	return Screen::MAIN_SCREEN;
}

/********************* Menu screen *********************/
Menu_Screen::Menu_Screen(){
	show_cursor = true;
	number_of_rows = MENU_SIZE;
	strcpy(text[0], " welcome            ");
	strcpy(text[1], " to the             ");
	strcpy(text[2], " menu screen        ");
	strcpy(text[3], " not much           ");
	strcpy(text[4], " to see right?      ");
}
Screen::SCREEN_ID Menu_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// button resets cursor row
	if(btn == Encoder::LONG_PUSH)
		return Screen::MAIN_SCREEN;
				
	// dir increments or decrements cursor row
	if(dir == Encoder::CLOCKWISE)
		increment_cursor();
	else if(dir == Encoder::COUNTERCLOCKWISE)
		decrement_cursor();
	
	// don't change screens
	return Screen::MENU_SCREEN;
}

/********************* Test screen *********************/
Test_Screen::Test_Screen(){
	show_cursor = false;
	number_of_rows = TEST_SIZE;
	strcpy(text[0], "test screen         ");
}
Screen::SCREEN_ID Test_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// button resets cursor row
	if(btn == Encoder::LONG_PUSH)
		return Screen::MAIN_SCREEN;
	
	// don't change screens
	return Screen::TEST_SCREEN;
}
