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

/********************* VL screen *********************/
VL_Screen::VL_Screen(LoadRegulator::LR_state &LR_state_r, TempRegulator::TR_state &TR_state_r): 
	_LR_state(LR_state_r),
	_TR_state(TR_state_r)
{
	// Initialize data
	row_offset = 0;
	cursor_row = 0;
	cursor_row_min = 0;
	show_cursor = false;
	number_of_rows = VL_SIZE;
	
	strcpy(text[0], "00.00 V\n");
	strcpy(text[1], "00.00 A\n");
	strcpy(text[2], "000.00 W\n");
	strcpy(text[3], "OFF             000%");
	
}
void VL_Screen::update_text(){
	// Update voltage, current, power, mode of variable load
	// Update Duty cycle of fan
	// Update measured voltage
	dtostrf(_LR_state._measured_voltage, SET_UI_MEASURED_VOLT_WIDTH, SET_UI_MEASURED_VOLT_DECIMAL, text[0]);
	strcat(text[0], " V\n");
	// Update measured current
	dtostrf(_LR_state._measured_current, SET_UI_MEASURED_CURR_WIDTH, SET_UI_MEASURED_CURR_DECIMAL, text[1]);
	strcat(text[1], " A\n");
	// Update measured power
	float measured_power = _LR_state._measured_voltage * _LR_state._measured_current;
	dtostrf(measured_power, SET_UI_MEASURED_POW_WIDTH, SET_UI_MEASURED_POW_DECIMAL, text[2]);
	strcat(text[2], " W\n");
	// Update mode
	switch(_LR_state._op_mode){
		case(LoadRegulator::CC):
			strcpy(text[3], "CC ");
			break;
		case(LoadRegulator::CP):
			strcpy(text[3], "CP ");
			break;
		case(LoadRegulator::CR):
			strcpy(text[3], "CR ");
			break;
		case(LoadRegulator::CV):
			strcpy(text[3], "CV ");
			break;
		case(LoadRegulator::OFF):
			strcpy(text[3], "OFF");
			break;
	}
	// Update duty cycle
	strcat(text[3], "            ");
	itoa(_TR_state._duty_cycle, text[3]+15, 10);
	strcat(text[3], "%\n");
}
Screen::SCREEN_ID VL_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// Push toggle output enable
	if(btn == Encoder::PUSH){
		// Toggle output enable, don't change screen
		asm("nop");
		return Screen::VL_SCREEN;
	}
	
	// Long push enters main menu
	if(btn == Encoder::LONG_PUSH){
		// Go to Main menu
		return Screen::MAIN_MENU_SCREEN;
	}
	
	// If no input, remain on current screen
	return Screen::VL_SCREEN;
}
/********************* Main Menu screen *********************/
Main_Menu_Screen::Main_Menu_Screen(){
	// Initialize data
	row_offset = 0;
	cursor_row = 1;
	cursor_row_min = 1;
	show_cursor = true;
	number_of_rows = MAIN_MENU_SIZE;
	
	strcpy(text[0], "Main menu\n");
	strcpy(text[1], " Set mode\n");
	strcpy(text[2], " Set target\n");
	strcpy(text[3], " Debugger\n");
	strcpy(text[4], " Fan Control\n");
	strcpy(text[5], " Info\n");
	
}
void Main_Menu_Screen::update_text(){
	// No need to update text
}
Screen::SCREEN_ID Main_Menu_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// Push to go to submenu
	if(btn == Encoder::PUSH){
		// Change screen; WIP
		switch(get_cursor()){
			case(1):
				// Go to LR mode screen
				return Screen::LR_MODE_SCREEN;
			case(2):
				// Go to LR val screen
				return Screen::LR_VAL_SCREEN;
			default:
				// Shouldn't be here; return to VL screen
				return Screen::VL_SCREEN;
		}
	}
	
	// Long push returns to VL screen
	if(btn == Encoder::LONG_PUSH){
		// Go to VL Screen
		return Screen::VL_SCREEN;
	}
	
	// dir increments or decrements cursor row
	if(dir == Encoder::CLOCKWISE)
		increment_cursor();
	else if(dir == Encoder::COUNTERCLOCKWISE)
		decrement_cursor();
	
	// don't change screens
	return Screen::MAIN_MENU_SCREEN;
}
/********************* LR Mode screen *********************/
LR_Mode_Screen::LR_Mode_Screen(LoadRegulator::LR_state &LR_state_r):
	_LR_state(LR_state_r)
{
	// Initialize data
	row_offset = 0;
	cursor_row = 1;
	cursor_row_min = 1;
	show_cursor = true;
	number_of_rows = LR_MODE_SIZE;
	
	strcpy(text[0], "LR Mode\n");
	strcpy(text[1], " CC\n");
	strcpy(text[2], " CP\n");
	strcpy(text[3], " CR\n");
	strcpy(text[4], " CV\n");
	strcpy(text[5], " OFF\n");

}
void LR_Mode_Screen::update_text(){
	// No need to update text
}
Screen::SCREEN_ID LR_Mode_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// Push to select mode
	if(btn == Encoder::PUSH){
		// Update load regulator mode
		switch(get_cursor()){
			case(1):
				_LR_state._op_mode = LoadRegulator::CC;
				break;
			case(2):
				_LR_state._op_mode = LoadRegulator::CP;
				break;
			case(3):
				_LR_state._op_mode = LoadRegulator::CR;
				break;
			case(4):
				_LR_state._op_mode = LoadRegulator::CV;
				break;
			case(5):
				_LR_state._op_mode = LoadRegulator::OFF;
				break;
			default:
				// Shouldn't be here; do nothing
				break;	
		}
		// Enable update and return to main menu
		_LR_state._update = true;
		return Screen::MAIN_MENU_SCREEN;
	}
	
	// Long push returns to VL screen
	if(btn == Encoder::LONG_PUSH){
		// Go to VL Screen
		return Screen::VL_SCREEN;
	}
	
	// dir increments or decrements cursor row
	if(dir == Encoder::CLOCKWISE)
		increment_cursor();
	else if(dir == Encoder::COUNTERCLOCKWISE)
		decrement_cursor();
	
	// don't change screens
	return Screen::LR_MODE_SCREEN;
}
/********************* LR Val screen *********************/
LR_Val_Screen::LR_Val_Screen(LoadRegulator::LR_state &LR_state_r):
	_LR_state(LR_state_r)
{
	// Initialize data
	row_offset = 0;
	cursor_row = 1;
	cursor_row_min = 1;
	show_cursor = false;
	number_of_rows = LR_VAL_SIZE;
	update_local_val = true;
	local_target_val = 0;
	
	strcpy(text[0], "Set target value: \n");
	strcpy(text[1], "           000.00 A\n");

}
void LR_Val_Screen::update_text(){
	// Update local value if necessary
	if(update_local_val){
		// Update local value from actual target value
		update_local_val = false;
		switch(_LR_state._op_mode){
			case(LoadRegulator::CC):
				local_target_val = _LR_state._target_current;
				break;
			case(LoadRegulator::CP):
				local_target_val = _LR_state._target_power;
				break;
			case(LoadRegulator::CR):
				local_target_val = _LR_state._target_resistance;
				break;
			case(LoadRegulator::CV):
				local_target_val = _LR_state._target_voltage;
				break;
			default:
				// Shouldn't be here
				local_target_val = 0;
				break;
		}
	}
	
	// Update text to show local value
	dtostrf(local_target_val, SET_UI_LOCAL_TARGET_WIDTH, SET_UI_LOCAL_TARGET_DECIMAL, text[1]);
	switch(_LR_state._op_mode){
		case(LoadRegulator::CC):
			strcat(text[1], " A\n");
			break;
		case(LoadRegulator::CP):
			strcat(text[1], " W\n");
			break;
		case(LoadRegulator::CR):
			strcat(text[1], " R\n");
			break;
		case(LoadRegulator::CV):
			strcat(text[1], " V\n");
			break;
		default:
			// Shouldn't be here
			strcat(text[1], "\n");
			break;
	}
}
Screen::SCREEN_ID LR_Val_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// Push to save value
	if(btn == Encoder::PUSH){
		// Write local to actual
		switch(_LR_state._op_mode){
			case(LoadRegulator::CC):
				_LR_state._target_current = local_target_val;
				break;
			case(LoadRegulator::CP):
				_LR_state._target_current = local_target_val;
				break;
			case(LoadRegulator::CR):
				_LR_state._target_current = local_target_val;
				break;
			case(LoadRegulator::CV):
				_LR_state._target_current = local_target_val;
				break;
			default:
				// Do nothing
				break;
		}
		// Re-enable getting local val before exiting
		update_local_val = true;
		// Enable update and return to main menu
		_LR_state._update = true;
		return Screen::MAIN_MENU_SCREEN;
	}
	
	// Long push returns to VL screen
	if(btn == Encoder::LONG_PUSH){
		// Go to VL Screen
		return Screen::VL_SCREEN;
	}
	
	// dir increments or decrements local target value
	if(dir == Encoder::CLOCKWISE)
		local_target_val += 0.1;
	else if(dir == Encoder::COUNTERCLOCKWISE)
		local_target_val -= 0.1;
	
	// don't change screens
	return Screen::LR_VAL_SCREEN;
}
