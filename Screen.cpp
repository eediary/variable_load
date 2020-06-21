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
		// Copy text to screen_chars if text should be visible on screen
		if(row_offset+i < number_of_rows){
			// Copy and paste
			strcpy(screen_chars[i], text[row_offset + i]);
			
			// Fill rest of line with blanks
			for(int j = strlen(screen_chars[i]); j < SCH_UI_LCD_COLS; j++){
				screen_chars[i][j] = ' ';
			}
		} else
			// If no text should be visible, fill with blank lines
			strcpy(screen_chars[i], BLANK_LINE);
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
	
	strcpy(text[0], VL_SCREEN_LINE_0);
	strcpy(text[1], VL_SCREEN_LINE_1);
	strcpy(text[2], VL_SCREEN_LINE_2);
	strcpy(text[3], VL_SCREEN_LINE_3);
	
}
void VL_Screen::res_to_text(float val, char* dest, int width, int prec){
	// Writes val to dest as text, with appropriate units and formatting
	// Reduce va as necessary
	int res_units_index = 0;
	char res_units[3][4] = { "  R", " kR", " MR"};
	res_units[0][2] = OHM_ICON; // replace R with ?
	res_units[1][2] = OHM_ICON;
	res_units[2][2] = OHM_ICON;
	if(val > 1E6){
		// In mega ohm range
		val /= 1E6;
		res_units_index = 2;
		} else if(val > 1E3){
		// In kilo ohm range
		val /= 1E3;
		res_units_index = 1;
		} else{
		// In ohm range
		// do nothing
	}
	dtostrf(val, width, prec, dest);
	strcat(dest, res_units[res_units_index]);
}
void VL_Screen::update_text(){
	// Update voltage, current, power, mode of variable load
	// Update load temperature
	
	// temp buffer to store strings
	char temp_buffer[SCH_UI_LCD_COLS];
	
	// Limit display voltage, current, power and resistance to non-negative numbers
	float display_voltage = (_LR_state._measured_voltage < 0) ? 0 : _LR_state._measured_voltage;
	float display_current = (_LR_state._measured_current < 0) ? 0 : _LR_state._measured_current;
	float display_power = display_voltage * display_current;
	float display_resistance = display_voltage / display_current;
	
	// Line 1: display voltage and power
	dtostrf(display_voltage, SET_UI_MEASURED_VOLT_WIDTH, SET_UI_MEASURED_VOLT_DECIMAL, text[0]);
	strcat(text[0], " V  ");
	dtostrf(display_power, SET_UI_MEASURED_POW_WIDTH, SET_UI_MEASURED_POW_DECIMAL, temp_buffer);
	strcat(temp_buffer, "  W");
	strcat(text[0], temp_buffer);
	
	// Line 2: display current and resistance
	dtostrf(display_current, SET_UI_MEASURED_CURR_WIDTH, SET_UI_MEASURED_CURR_DECIMAL, text[1]);
	strcat(text[1], " A     ");
	res_to_text(display_resistance, temp_buffer, SET_UI_MEASURED_RES_WIDTH, SET_UI_MEASURED_RES_DECIMAL);
	strcat(text[1], temp_buffer);
	
	// Line 3: display mode and target value
	// Update mode
	switch(_LR_state._op_mode){
		case(LoadRegulator::CC):
			strcpy(text[2], "CC: ");
			dtostrf(_LR_state._target_current, SET_UI_TARGET_CURR_WIDTH, SET_UI_TARGET_CURR_DECIMAL, temp_buffer);
			strcat(text[2], temp_buffer);
			strcat(text[2], " A");
			break;
		case(LoadRegulator::CP):
			strcpy(text[2], "CP: ");
			dtostrf(_LR_state._target_power, SET_UI_TARGET_POW_WIDTH, SET_UI_TARGET_POW_DECIMAL, temp_buffer);
			strcat(text[2], temp_buffer);
			strcat(text[2], " W");
			break;
		case(LoadRegulator::CR):
			strcpy(text[2], "CR: ");
			res_to_text(_LR_state._target_resistance, temp_buffer, SET_UI_TARGET_RES_WIDTH, SET_UI_TARGET_RES_DECIMAL);
			strcat(text[2], temp_buffer);
			break;
		case(LoadRegulator::CV):
			strcpy(text[2], "CV: ");
			dtostrf(_LR_state._target_voltage, SET_UI_TARGET_VOLT_WIDTH, SET_UI_TARGET_VOLT_DECIMAL, temp_buffer);
			strcat(text[2], temp_buffer);
			strcat(text[2], " V");
			break;
		case(LoadRegulator::OFF):
			strcpy(text[2], "OFF");
			break;
	}
	
	// Line 4: display temperature and duty cycle
	dtostrf(_TR_state._temp, SET_UI_TEMP_WIDTH, SET_UI_TEMP_DECIMAL, text[3]);
	strcat(text[3], "  C     ");
	text[3][SET_UI_TEMP_WIDTH + 1] = DEG_ICON;
	if(_TR_state._enable)
		strcat(text[3], "A: ");
	else
		strcat(text[3], "M: ");
	dtostrf(_TR_state._current_duty_cycle, 3, 0, temp_buffer);
	strcat(text[3], temp_buffer);
	strcat(text[3], " %");
}
Screen::SCREEN_ID VL_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	static LoadRegulator::operation_mode last_mode = LoadRegulator::OFF;
	// Push toggle output enable
	if(btn == Encoder::PUSH){
		// Toggle output enable, don't change screen
		if(_LR_state._op_mode == LoadRegulator::OFF){
			// Mode is OFF, so update to old mode
			_LR_state._op_mode = last_mode;
			_LR_state._update = true;
		} else{
			// Mode is not oFF, so save mode then disable
			last_mode = _LR_state._op_mode;
			_LR_state._op_mode = LoadRegulator::OFF;
			_LR_state._update = true;
		}
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
	cursor_row_min = 0;
	show_cursor = true;
	number_of_rows = MAIN_MENU_SIZE;
	
	strcpy(text[0], MAIN_MENU_LINE_0);
	strcpy(text[1], MAIN_MENU_LINE_1);
	strcpy(text[2], MAIN_MENU_LINE_2);
	strcpy(text[3], MAIN_MENU_LINE_3);
	strcpy(text[4], MAIN_MENU_LINE_4);
	
}
void Main_Menu_Screen::update_text(){
	// No need to update text
}
Screen::SCREEN_ID Main_Menu_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// Push to go to submenu
	if(btn == Encoder::PUSH){
		// Change screen; WIP
		switch(get_cursor()){
			case(0):
				// Go to VL screen
				return Screen::VL_SCREEN;
			case(1):
				// Go to LR mode screen
				return Screen::LR_MODE_SCREEN;
			case(2):
				// Go to LR val screen
				return Screen::LR_VAL_SCREEN;
			case(3):
				// Go to TR val screen
				return Screen::TR_VAL_SCREEN;
			case(4):
				// Go to Info screen
				return Screen::INFO_SCREEN;
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
	use_local_op_mode = false;
	digit_index = 0;
	select_digit = true;
	display_width = SET_UI_TARGET_CURR_WIDTH;
	display_dec = SET_UI_TARGET_CURR_DECIMAL;
	local_val_max = SET_LR_CUR_MAX;
	
	strcpy(text[0], LR_VAL_LINE_0_A);
	strcpy(text[1], LR_VAL_LINE_1);
	strcpy(text[2], LR_VAL_LINE_2);

}
void LR_Val_Screen::update_text(){
	// use local op mode if coming from LR mode screen, or actual op mode if coming from main menu
	LoadRegulator::operation_mode mode = (use_local_op_mode) ? local_op_mode : _LR_state._op_mode;
	
	// Update local value if necessary
	if(update_local_val){
		// Update local value from actual target value
		update_local_val = false;
		switch(mode){
			case(LoadRegulator::CC):
				local_target_val = _LR_state._target_current;
				display_width = SET_UI_LOCAL_TARGET_CUR_WIDTH;
				display_dec = SET_UI_LOCAL_TARGET_CUR_DECIMAL;
				local_val_max = SET_LR_CUR_MAX;
				break;
			case(LoadRegulator::CP):
				local_target_val = _LR_state._target_power;
				display_width = SET_UI_LOCAL_TARGET_POW_WIDTH;
				display_dec = SET_UI_LOCAL_TARGET_POW_DECIMAL;
				local_val_max = SET_LR_POW_MAX;
				break;
			case(LoadRegulator::CR):
				local_target_val = _LR_state._target_resistance;
				display_width = SET_UI_LOCAL_TARGET_RES_WIDTH;
				display_dec = SET_UI_LOCAL_TARGET_RES_DECIMAL;
				local_val_max = SET_LR_RES_MAX;
				break;
			case(LoadRegulator::CV):
				local_target_val = _LR_state._target_voltage;
				display_width = SET_UI_LOCAL_TARGET_VOLT_WIDTH;
				display_dec = SET_UI_LOCAL_TARGET_VOLT_DECIMAL;
				local_val_max = SET_LR_VOLT_MAX;
				break;
			default:
				// Shouldn't be here
				local_target_val = 0;
				break;
		}
	}
	
	// Update header
	if(select_digit){
		// in select digit mode
		strcpy(text[0], LR_VAL_LINE_0_A);
	} else{
		// in modify digit mode
		strcpy(text[0], LR_VAL_LINE_0_B);
	}
	// Update text to show local value
	dtostrf(local_target_val, display_width, display_dec, text[1]);
	switch(mode){
		case(LoadRegulator::CC):
			strcat(text[1], " A          END");
			break;
		case(LoadRegulator::CP):
			strcat(text[1], " W         END");
			break;
		case(LoadRegulator::CR):
			strcat(text[1], " R         END");
			break;
		case(LoadRegulator::CV):
			strcat(text[1], " V          END");
			break;
		default:
			// Shouldn't be here
			strcat(text[1], "");
			break;
	}
	
	// Display which digit is currently selected
	strcpy(text[2], BLANK_LINE);
	if(digit_index == display_width)
		// Set cursor on END
		text[2][LR_VAL_END_OFFSET] = '^';
	else
		// Display cursor on digit
		text[2][digit_index] = '^';
}
Screen::SCREEN_ID LR_Val_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// use local op mode if coming from LR mode screen, or actual op mode if coming from main menu
	LoadRegulator::operation_mode mode = (use_local_op_mode) ? local_op_mode : _LR_state._op_mode;
	
	// Push to switch modes or save value
	if(btn == Encoder::PUSH){
		if(digit_index == display_width){
			// cursor on END, so save value
			// Use either local or actual op mode
			switch(mode){
				case(LoadRegulator::CC):
					_LR_state._target_current = local_target_val;
					_LR_state._op_mode = LoadRegulator::CC;
					break;
				case(LoadRegulator::CP):
					_LR_state._target_power = local_target_val;
					_LR_state._op_mode = LoadRegulator::CP;
					break;
				case(LoadRegulator::CR):
					_LR_state._target_resistance = local_target_val;
					_LR_state._op_mode = LoadRegulator::CR;
					break;
				case(LoadRegulator::CV):
					_LR_state._target_voltage = local_target_val;
					_LR_state._op_mode = LoadRegulator::CV;
					break;
				default:
					// Do nothing for OFF
					break;
			}
			// Re-enable getting local val before exiting
			update_local_val = true;
			// Disable using local op mode in case coming directly from main menu
			use_local_op_mode = false;
			// Enable update and return to main menu
			_LR_state._update = true;
			// Reset cursor and digit selection
			digit_index = 0;
			select_digit = true;
			return Screen::MAIN_MENU_SCREEN;
		} else{
			// Since not at END, switch between selecting and modifying digit
			select_digit = !select_digit;
		}
	}
	
	// Long push returns to VL screen
	if(btn == Encoder::LONG_PUSH){
		// Re-enable getting local val before exiting
		update_local_val = true;
		// Disable using local op mode in case coming directly from main menu
		use_local_op_mode = false;
		// Go to VL Screen
		return Screen::VL_SCREEN;
	}
	
	// dir increments or decrements cursor in select digit
	// dir increments or decrements digit in modify digit
	if(select_digit){
		// In select digit mode; allows user to move cursor
		if(dir == Encoder::CLOCKWISE)
			digit_index++;
		else if(dir == Encoder::COUNTERCLOCKWISE)
			digit_index--;
		// skip decimal
		if(digit_index == display_width - display_dec - 1){
			if(dir == Encoder::CLOCKWISE)
				digit_index++;
			else if(dir == Encoder::COUNTERCLOCKWISE)
				digit_index--;
		}
		// limit cursor
		if(digit_index < 0)
			digit_index = 0;
		else if(digit_index > display_width)
			digit_index = display_width;
	} else{
		// In modify digit mode; modify local val
		// local val is increased by 10^x, where x depends on location of cursor
		// calculate x when cursor is left of decimal point
		int exponent = display_width - display_dec - 2 - digit_index;
		// adjust x for when cursor is right of decimal point
		if(digit_index >= display_width - display_dec - 1)
			exponent += 1;
		// calculate 10^x
		float multiplier = pow(10,exponent);
		// adjust value
		if(dir == Encoder::CLOCKWISE)
			local_target_val += multiplier;
		else if(dir == Encoder::COUNTERCLOCKWISE)
			local_target_val -= multiplier;
		// limit local target val
		if(local_target_val < 0)
			local_target_val = 0;
		else if(local_target_val > local_val_max)
			local_target_val = local_val_max;
	}
	
	// go to LR mode screen if mode is off; otherwise remain on screen
	if(mode == LoadRegulator::OFF){
		// Re-enable getting local val before exiting
		update_local_val = true;
		// Disable using local op mode in case coming directly from main menu
		use_local_op_mode = false;
		return Screen::LR_MODE_SCREEN;
	}else
		return Screen::LR_VAL_SCREEN;
}
void LR_Val_Screen::update_op_mode(LoadRegulator::operation_mode mode){
	// Used in LR mode screen
	local_op_mode = mode;
	use_local_op_mode = true;
}
LoadRegulator::operation_mode LR_Val_Screen::get_op_mode(){
	return local_op_mode;
}
/********************* LR Mode screen *********************/
LR_Mode_Screen::LR_Mode_Screen(LoadRegulator::LR_state &LR_state_r, LR_Val_Screen &LR_Val_Screen_r):
_LR_state(LR_state_r),
_LR_Val_Screen(LR_Val_Screen_r)
{
	// Initialize data
	row_offset = 0;
	cursor_row = 0;
	cursor_row_min = 0;
	show_cursor = true;
	number_of_rows = LR_MODE_SIZE;
	
	strcpy(text[0], LR_MODE_LINE_0);
	strcpy(text[1], LR_MODE_LINE_1);
	strcpy(text[2], LR_MODE_LINE_2);
	strcpy(text[3], LR_MODE_LINE_3);
	strcpy(text[4], LR_MODE_LINE_4);
	strcpy(text[5], LR_MODE_LINE_5);

}
void LR_Mode_Screen::update_text(){
	// No need to update text
}
Screen::SCREEN_ID LR_Mode_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// Push to select mode
	if(btn == Encoder::PUSH){
		// Update local op mode in LR Val screen for non-OFF modes
		switch(get_cursor()){
			case(0):
			// Cancel; do nothing
			break;
			case(1):
				_LR_Val_Screen.update_op_mode(LoadRegulator::CC);
				break;
			case(2):
				_LR_Val_Screen.update_op_mode(LoadRegulator::CP);
				break;
			case(3):
				_LR_Val_Screen.update_op_mode(LoadRegulator::CR);
				break;
			case(4):
				_LR_Val_Screen.update_op_mode(LoadRegulator::CV);
				break;
			case(5):
				_LR_state._op_mode = LoadRegulator::OFF;
				break;
			default:
				// Shouldn't be here; do nothing
				break;
		}
		// If cancel was selected, do nothing; return to main menu
		if(get_cursor() == 0)
			return Screen::MAIN_MENU_SCREEN;
		else{
			// If OFF is selected, update and return to main menu. Otherwise go to LR Val screen
			if(get_cursor() == 5){
				_LR_state._update = true;
				return Screen::MAIN_MENU_SCREEN;
			}else{
				_LR_state._update = false;
				return Screen::LR_VAL_SCREEN;
			}
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
	return Screen::LR_MODE_SCREEN;
}
/********************* TR Val screen *********************/
TR_Val_Screen::TR_Val_Screen(TempRegulator::TR_state &TR_state_r):
_TR_state(TR_state_r)
{
	// Initialize data
	row_offset = 0;
	cursor_row = 0;
	cursor_row_min = 0;
	show_cursor = false;
	number_of_rows = TR_VAL_SIZE;
	
	strcpy(text[0], TR_VAL_LINE_0);	
}
int TR_Val_Screen::index_to_duty_cycle(){
	// if Index = 0, output is irrelevant
	// Otherwise, duty cycle = (index - 1) * 5, between 0 and 100 inclusive
	int to_return = (index - 1) * 5;
	if(to_return < 0)
		to_return = 0;
	else if(to_return > 100)
		to_return = 100;
	return to_return;
}
void TR_Val_Screen::update_text(){
	// Update value that is displayed
	if(index){
		// index is non-zero; display number
		itoa(index_to_duty_cycle(), text[1], 10);
		strcat(text[1], " %");
	} else{
		// index is zero; display AUTO
		strcpy(text[1], "AUTO");
	}
}
Screen::SCREEN_ID TR_Val_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// Push to update TR state
	if(btn == Encoder::PUSH){
		// Update TR state, then return to main menu
		if(index){
			// put TR into manual mode, adjust duty cycle
			_TR_state._target_duty_cycle = index_to_duty_cycle();
			_TR_state._enable = false;
		} else{
			// put TR into auto mode
			_TR_state._enable = true;
		}
		_TR_state._update = true;
		return MAIN_MENU_SCREEN;
	}
	
	// Long push returns to VL screen
	if(btn == Encoder::LONG_PUSH){
		// Go to VL screen
		return Screen::VL_SCREEN;
	}
	
	// dir increments or decrements index
	if(dir == Encoder::CLOCKWISE)
		index++;
	else if(dir == Encoder::COUNTERCLOCKWISE)
		index--;
	
	// bound index
	if(index < 0)
		index = 0;
	else if(index > 21)
		index = 21;
	
	// don't change screens
	return Screen::TR_VAL_SCREEN;
}
/********************* Info screen *********************/
Info_Screen::Info_Screen()
{
	// Initialize data
	row_offset = 0;
	cursor_row = 0;
	cursor_row_min = 0;
	show_cursor = false;
	number_of_rows = INFO_SIZE;
	strcpy(text[0], INFO_LINE_0);
	strcpy(text[1], INFO_LINE_1);
	strcpy(text[2], INFO_LINE_2);
	
}
void Info_Screen::update_text(){
	// no need to update text
}
Screen::SCREEN_ID Info_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// Push to go back to main menu
	if(btn == Encoder::PUSH){
		return MAIN_MENU_SCREEN;
	}
	
	// Long push returns to VL screen
	if(btn == Encoder::LONG_PUSH){
		// Go to VL screen
		return Screen::VL_SCREEN;
	}
	
	// dir does nothing
// 	if(dir == Encoder::CLOCKWISE)
// 		increment_cursor();
// 	else if(dir == Encoder::COUNTERCLOCKWISE)
// 		decrement_cursor();
	
	// don't change screens
	return Screen::INFO_SCREEN;
}