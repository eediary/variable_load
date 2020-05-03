#pragma once

#include <avr/io.h>
class HAL_GPIO
{
public: // enums
	enum GPIO_port{
		GPIO_PORTB, GPIO_PORTC, GPIO_PORTD, GPIO_PORTE, GPIO_PORTF
	};
	enum GPIO_pin {
		GPIO_PIN0 = 0, GPIO_PIN1, GPIO_PIN2, GPIO_PIN3, GPIO_PIN4, GPIO_PIN5, GPIO_PIN6, GPIO_PIN7
	};
	enum GPIO_dir{
		GPIO_INPUT, GPIO_OUTPUT
	};
	enum GPIO_INT{
		GPIO_NONE, GPIO_PCINT, GPIO_EXTINT, GPIO_EXTINT6
	};
	enum GPIO_INT_TRIG{
		GPIO_LOW = 0b00, GPIO_EDGE = 0b01, GPIO_FALLING = 0b10, GPIO_RISING = 0b11
	};

private:
	uint8_t gpio_pin;
	uint8_t gpio_pin_m;
	// can't use references to registers since registers must be initialized correctly
	volatile uint8_t *gpio_DDR_r;
	volatile uint8_t *gpio_PORT_r;
	volatile uint8_t *gpio_PIN_r;
	GPIO_INT int_type;
	
public:
	HAL_GPIO(GPIO_port port, GPIO_pin pin, GPIO_dir dir = GPIO_INPUT, uint8_t val = 0){
		// constructor; sets up PORT, PIN and Data Direction
		// also writes value to PORT if val is not zero
		// Default: input, low
		gpio_pin = pin;
		gpio_pin_m = (1 << pin);
		int_type = GPIO_NONE;
		switch(port){
			case GPIO_PORTB:
			gpio_PORT_r = &PORTB;
			gpio_DDR_r = &DDRB;
			gpio_PIN_r = &PINB;
			int_type = GPIO_PCINT;
			break;
			case GPIO_PORTC:
			gpio_PORT_r = &PORTC;
			gpio_DDR_r = &DDRC;
			gpio_PIN_r = &PINC;
			break;
			case GPIO_PORTD:
			gpio_PORT_r = &PORTD;
			gpio_DDR_r = &DDRD;
			gpio_PIN_r = &PIND;
			if(pin < GPIO_PIN4)
			int_type = GPIO_EXTINT;
			break;
			case GPIO_PORTE:
			gpio_PORT_r = &PORTE;
			gpio_DDR_r = &DDRE;
			gpio_PIN_r = &PINE;
			if(pin == GPIO_PIN6)
			int_type = GPIO_EXTINT6;
			break;
			case GPIO_PORTF:
			gpio_PORT_r = &PORTF;
			gpio_DDR_r = &DDRF;
			gpio_PIN_r = &PINF;
			break;
		}
		// Set up pin direction and initial value
		if(val){
			set_pin();
			} else{
			clear_pin();
		}
		set_direction(dir);
	}
	void set_direction(GPIO_dir dir){
		if(dir == GPIO_INPUT){
			*gpio_DDR_r &= ~gpio_pin_m;
			} else if(dir == GPIO_OUTPUT){
			*gpio_DDR_r |= gpio_pin_m;
		}
	}
	bool is_output(){
		return (bool)(*gpio_DDR_r & gpio_pin_m);
	}
	void clear_pin(){
		*gpio_PORT_r &= ~gpio_pin_m;
	}
	void set_pin(){
		*gpio_PORT_r |= gpio_pin_m;
	}
	void toggle_pin(){
		*gpio_PORT_r ^= gpio_pin_m;
	}
	bool read_pin(){
		return (bool)(*gpio_PIN_r & gpio_pin_m);
	}
	void enable_pullup(){
		// enables pullup
		// WARNING: pullup affects all ports
		// for pin to be pulled-up, pin must be set as input AND have its bit in PORT be high
		MCUCR &= ~(1 << PUD);
	}
	void disable_pullup(){
		// disables pullup
		// WARNING: pullup affects all ports
		MCUCR |= 1 << PUD;
	}
	bool is_pullup(){
		return (bool)(!(MCUCR & (1 << PUD)));
	}
	int gpio_interrupt_enable(){
		// enables interrupt for pin
		// enables external interrupt or pin change interrupt
		// if pin can't generate interrupt, returns -1
		switch(int_type){
			case(GPIO_NONE):
			return -1;
			case(GPIO_PCINT):
			PCMSK0 |= gpio_pin_m;
			return 1;
			case(GPIO_EXTINT):
			case(GPIO_EXTINT6):
			EIMSK |= gpio_pin_m;
			return 2;
		}
		return 0; // should not reach here
	}
	int gpio_interrupt_disable(){
		// disables interrupt for pin
		// disables external interrupt or pin change interrupt
		// if pin can't generate interrupt, returns -1
		switch(int_type){
			case(GPIO_NONE):
			return -1;
			case(GPIO_PCINT):
			PCMSK0 &= ~gpio_pin_m;
			return 1;
			case(GPIO_EXTINT):
			case(GPIO_EXTINT6):
			EIMSK &= ~gpio_pin_m;
			return 2;
		}
		return 0; // should not reach here
	}
	int gpio_extint_config(GPIO_INT_TRIG setting){
		// configures external interrupt trigger for pin
		// trigger can be rising edge, falling edge, edge or low
		// if pin is not external interrupt pin, returns 1
		if(int_type == GPIO_EXTINT){
			EICRA &= ~(0b11 << (gpio_pin * 2));
			EICRA |= setting << (gpio_pin * 2);
			return 0;
			} else if(int_type == GPIO_EXTINT6){
			EICRB &= ~(0b11 << (ISC60 * 2));
			EICRB |= setting << (ISC60 * 2);
			return 0;
		}
		return 1;
	}
};