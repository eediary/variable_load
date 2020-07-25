#include "User_Interface.h"

User_Interface::User_Interface(LoadRegulator &LR_ref, TempRegulator &TR_ref, HAL_Timer &Timer_r):
LR_r(LR_ref),
TR_r(TR_ref),
Timer(Timer_r),
Enc(SCH_UI_A_PORT, SCH_UI_A_PIN, SCH_UI_B_PORT, SCH_UI_B_PIN, SCH_UI_BTN_PORT, SCH_UI_BTN_PIN, Timer, SCH_UI_ENC_LEAD),
Lcd(SCH_UI_LCD_ADDR),
DBG_LED(SET_LED_UI_PORT, SET_LED_UI_PIN, SET_LED_UI_ACTIVE),
// screens
VL_screen(LR_ref, TR_ref),
Main_menu_screen(),
LR_Mode_screen(LR_ref, LR_Val_screen),
LR_Val_screen(LR_ref),
TR_Val_screen(TR_ref),
Info_screen()
{
	// Initialize variables
	cur_row = 0;
	cur_col = 0;
	last_update = Timer.get_tick();
	print_flag = false;
	cur_screen = &VL_screen;
	
	// Set up LCD
	Lcd.begin(SCH_UI_LCD_COLS, SCH_UI_LCD_ROWS);
	Lcd.setBacklight(255);
	Lcd.clear();
}
void User_Interface::update_screen(){
	// Get inputs
	Encoder::Encoder_Dir dir = Enc.get_dir();
	Encoder::Encoder_Button press = Enc.get_pressed();
	
	// Clear screen so it can be updated
	// Done periodically or when input changes
	unsigned long cur_time = Timer.get_tick();
	if(
	(cur_time - last_update > SET_UI_LCD_UPDATE_PERIOD) ||
	(dir != Encoder::NONE) ||
	(press != Encoder::NO_PUSH))
	{
		// DEBUG: turn LED on
		DBG_LED.on();
		
		// Enough time has passed for screen to update, or user performed an action
		last_update = cur_time;
		
		// Reset cursor, col and row
		// Set flag
		Lcd.setCursor(0,0);
		cur_col = 0;
		cur_row = 0;
		print_flag = true;
		
		// Handle user input
		// Pass input to screen; update screen pointer
		switch(cur_screen->handle_input(dir, press)){
			case(Screen::VL_SCREEN):
				cur_screen = &VL_screen;
				break;
			case(Screen::MAIN_MENU_SCREEN):
				cur_screen = &Main_menu_screen;
				break;
			case(Screen::LR_MODE_SCREEN):
				cur_screen = &LR_Mode_screen;
				break;
			case(Screen::LR_VAL_SCREEN):
				cur_screen = &LR_Val_screen;
				break;
			case(Screen::TR_VAL_SCREEN):
				cur_screen = &TR_Val_screen;
				break;
			case(Screen::INFO_SCREEN):
				cur_screen = &Info_screen;
				break;
		}
		
		// Update characters to display on screen
		cur_screen->update_screen_chars(screen_chars);
	}
	
	// If enabled, print characters to screen
	if(print_flag){
		// Print screen_chars
		char cur_char = screen_chars[cur_row][cur_col];
		if(cur_col >= SCH_UI_LCD_COLS){
			// end of current line
			// either get next line, or stop printing since you're done with screen
			if(++cur_row >= SCH_UI_LCD_ROWS){
				// done printing screen; clear flag
				print_flag = false;
			} else{
				// get next line
				Lcd.setCursor(0, cur_row);
				cur_col = 0;
			}
		}
		else{
			// in the middle of line; keep printing
			Lcd.print(cur_char);
			cur_col++;
		}
	}
	
	// DEBUG: turn LED off
	DBG_LED.off();
}