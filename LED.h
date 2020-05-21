#pragma once

#include <avr/io.h>

#include "HAL_GPIO.h"

class LED
{
public: // enums
	enum LED_active{
		LED_ACTIVE_HIGH = 0,
		LED_ACTIVE_LOW = 1
	};

private:
	HAL_GPIO GPIO;
	LED_active active;
	
public:
	LED(HAL_GPIO::GPIO_port led_port, HAL_GPIO::GPIO_pin led_pin, LED_active active_sig) : 
	GPIO(led_port, led_pin, HAL_GPIO::GPIO_OUTPUT, active_sig),
	active(active_sig)
	{
		// constructor: runs initialization list
	}
	void off(){
		if(active == LED_ACTIVE_HIGH)
			GPIO.clear_pin();
		else
			GPIO.set_pin();
	}
	void on(){
		if(active == LED_ACTIVE_HIGH)
			GPIO.set_pin();
		else
			GPIO.clear_pin();
	}
	void toggle(){
		GPIO.toggle_pin();
	}
	void disable(){
		GPIO.set_direction(HAL_GPIO::GPIO_INPUT);
	}
	void enable(){
		GPIO.set_direction(HAL_GPIO::GPIO_OUTPUT);
	}
};