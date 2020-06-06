#include "Screen.h"
/********************* Screen class *********************/
Screen::Screen(){
	// Initialize in derived classes
}
void Screen::update_screen_chars(char screen_chars[SCH_UI_LCD_ROWS][SCH_UI_LCD_COLS+1]){
	// Update text
	update_text();
	
	// Load text to screen_chars
	for(int i = 0; i < SCH_UI_LCD_ROWS; i++){
		// Copy text to screen_chars if possible
		int text_row = row_offset+i;
		if(text_row >= number_of_rows){
			// no need to print more
			screen_chars[i][0] = '\n';
		} else
			// Copy and paste
			strcpy(screen_chars[i], text[row_offset + i]);
	}
	
	// Load cursor
	if(show_cursor){
		screen_chars[cursor_row-row_offset][0] = CURSOR_ICON;
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
void Screen::limit_cursor(){
	if(cursor_row >= number_of_rows)
		cursor_row= number_of_rows - 1;
	if(cursor_row < cursor_row_min)
		cursor_row = cursor_row_min;
}
void Screen::increment_cursor(){
	cursor_row++;
	limit_cursor();
	update_row_offset();
}
void Screen::decrement_cursor(){
	cursor_row--;
	limit_cursor();
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
	// Initialize data
	row_offset = 0;
	cursor_row = 1;
	cursor_row_min = 1;
	show_cursor = true;
	number_of_rows = MAIN_SIZE;
	
	strcpy(text[0], "Main menu\n");
	strcpy(text[1], " Line 1\n");
	strcpy(text[2], " Line 2\n");
	strcpy(text[3], " Line 3\n");
	strcpy(text[4], " Line 4\n");
	strcpy(text[5], " Line 5\n");
	strcpy(text[6], " Line 6\n");
	strcpy(text[7], " Line 7\n");
	
}
void Main_Screen::update_text(){
	// No need to update
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

// /********************* Menu screen *********************/
// Menu_Screen::Menu_Screen(){
// 	show_cursor = true;
// 	number_of_rows = MENU_SIZE;
// 	strcpy(text[0], " welcome            ");
// 	strcpy(text[1], " to the             ");
// 	strcpy(text[2], " menu screen        ");
// 	strcpy(text[3], " not much           ");
// 	strcpy(text[4], " to see right?      ");
// }
// Screen::SCREEN_ID Menu_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
// 	// button resets cursor row
// 	if(btn == Encoder::LONG_PUSH)
// 		return Screen::MAIN_SCREEN;
// 				
// 	// dir increments or decrements cursor row
// 	if(dir == Encoder::CLOCKWISE)
// 		increment_cursor();
// 	else if(dir == Encoder::COUNTERCLOCKWISE)
// 		decrement_cursor();
// 	
// 	// don't change screens
// 	return Screen::MENU_SCREEN;
// }
// 
// /********************* Test screen *********************/
// Test_Screen::Test_Screen(){
// 	show_cursor = false;
// 	number_of_rows = TEST_SIZE;
// 	strcpy(text[0], "test screen         ");
// }
// Screen::SCREEN_ID Test_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
// 	// button resets cursor row
// 	if(btn == Encoder::LONG_PUSH)
// 		return Screen::MAIN_SCREEN;
// 	
// 	// don't change screens
// 	return Screen::TEST_SCREEN;
// }
