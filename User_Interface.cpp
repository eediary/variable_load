#include "User_Interface.h"

User_Interface::User_Interface(LoadRegulator::LR_state &LR_state_r, TempRegulator::TR_state &TR_state_r, HAL_Timer &Timer_r): 
	_LR_state(LR_state_r),
	_TR_state(TR_state_r),
	Timer(Timer_r),
	Enc(SCH_UI_A_PORT, SCH_UI_A_PIN, SCH_UI_B_PORT, SCH_UI_B_PIN, SCH_UI_BTN_PORT, SCH_UI_BTN_PIN, Timer, SCH_UI_ENC_LEAD),
	Lcd(SCH_UI_LCD_ADDR),
	VL_screen(_LR_state, _TR_state),
	Main_menu_screen(),
	LR_Mode_screen(_LR_state),
	LR_Val_screen(_LR_state),
	TR_Val_screen(_TR_state),
	Info_screen()
{
	// Initialize variables
	cur_row = 0;
	cur_col = 0;
	last_update = Timer.get_tick();
	wait_for_clear_flag = false;
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
	if(
		(Timer.get_tick() - last_update > SET_UI_LCD_UPDATE_PERIOD) || 
		(dir != Encoder::NONE) || 
		(press != Encoder::NO_PUSH))
	{
		// Reset cursor, col and row
		// Set and clear flags, preventing further execution until clear completes
		Lcd.setCursor(0,0);
		cur_col = 0;
		cur_row = 0;
		wait_for_clear_flag = true;
		print_flag = false;
		last_update = Timer.get_tick();
	}
	
	// Check to see if clear has completed
	if(wait_for_clear_flag && (Timer.get_tick() - last_update > SET_UI_LCD_CLEAR_PERIOD)){
		// Clear has completed
		// Update screen_chars, set print flag, clear and set flags
		cur_screen->update_screen_chars(screen_chars);
		wait_for_clear_flag = false;
		print_flag = true;
	}
	
	// If enabled, print characters to screen
	if(print_flag){
		// Print screen_chars
		char cur_char = screen_chars[cur_row][cur_col];
		if(cur_col >= SCH_UI_LCD_COLS){
			// end of current line
			// either get next line, or stop printing since you're done with screen
			if(++cur_row >= SCH_UI_LCD_ROWS){
				// done printing screen; reset col and row, flag and cursor
				print_flag = false;
				cur_col = 0;
				cur_row = 0;
				Lcd.setCursor(0,0);
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
}