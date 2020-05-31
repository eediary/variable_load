#include "User_Interface.h"

User_Interface::User_Interface(HAL_Timer &Timer_r): 
	Timer(Timer_r),
	Enc(SCH_UI_A_PORT, SCH_UI_A_PIN, SCH_UI_B_PORT, SCH_UI_B_PIN, SCH_UI_BTN_PORT, SCH_UI_BTN_PIN, Timer, SCH_UI_ENC_LEAD),
	Lcd(SCH_UI_LCD_ADDR)
{
	// Initialize variables
	cur_row = 0;
	cur_col = 0;
	last_update = Timer.get_tick();
	print_flag = false;
	cur_screen = &Main_screen;
	
	// Set up LCD
	Lcd.begin(SCH_UI_LCD_COLS, SCH_UI_LCD_ROWS);
	Lcd.setBacklight(255);
	Lcd.clear();
}
void User_Interface::update_screen(){
	// Handle updating LCD
	if(Timer.get_tick() - last_update > SET_UI_LCD_UPDATE_PERIOD){
		// Enough time has passed for screen to update, then enable flag
		last_update = Timer.get_tick();
		print_flag = true;
	}
	if(print_flag){
		// Print to 20 x 4 screen
		Lcd.setCursor(cur_col, cur_row);
		Lcd.print(cur_screen->get_char(cur_col++, cur_row));
		// Get next row and col
		if(cur_col >= SCH_UI_LCD_COLS){
			// reach end of line; get next line
			cur_col = 0;
			cur_row++;
		}
		if(cur_row >= SCH_UI_LCD_ROWS){
			// reach end of screen; wait for next screen
			cur_col = 0;
			cur_row = 0;
			print_flag = false;
		}
	}
	// Pass input to screen; update screen pointer
	switch(cur_screen->handle_input(Enc.get_dir(), Enc.get_pressed())){
		case(Screen::MAIN_SCREEN):
			cur_screen = &Main_screen;
			break;
		case(Screen::MENU_SCREEN):
			cur_screen = &Menu_screen;
			break;
		case(Screen::TEST_SCREEN):
			cur_screen = &Test_screen;
			break;
	}
}