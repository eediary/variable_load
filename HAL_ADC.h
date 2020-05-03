#pragma once

#include <avr/io.h>

#define DIDR_MASK (0b0011111111110011)

class HAL_ADC{
public:
	enum ADC_voltage_reference{
		ADC_AREF = 0b00, ADC_AVCC = 0b01, ADC_IREF = 0b11
	};
	enum ADC_adjust{
		ADC_RADJUST = 0b0, ADC_LADJUST = 0b1
	};
	enum ADC_channel{
		ADC_ADC0 =  0b000000,
		ADC_ADC1 =  0b000001,
		ADC_ADC4 =  0b000100,
		ADC_ADC5 =  0b000101,
		ADC_ADC6 =  0b000110,
		ADC_ADC7 =  0b000111,
		ADC_ADC8 =  0b100000,
		ADC_ADC9 =  0b100001,
		ADC_ADC10 = 0b100010,
		ADC_ADC11 = 0b100011,
		ADC_ADC12 = 0b100100,
		ADC_ADC13 = 0b100101,
		
		ADC_BANDGAP = 0b011110,
		ADC_GND = 0b011111,
		ADC_TEMP = 100111
	};
	enum ADC_prescaler{
		ADC_DIV2 = 0b001,
		ADC_DIV4 = 0b010,
		ADC_DIV8 = 0b011,
		ADC_DIV16 = 0b100,
		ADC_DIV32 = 0b101,
		ADC_DIV64 = 0b110,
		ADC_DIV128 = 0b111
	};
private:
public:
	HAL_ADC(ADC_prescaler prescaler, ADC_voltage_reference vref_sel, ADC_adjust adjust, uint16_t adc_mask){
		// constructor
		// disable ADC, configure it, enable it
		disable_adc();
		set_prescaler(prescaler);
		set_reference(vref_sel);
		set_adjust(adjust);
		disable_digital_input(adc_mask);
		enable_adc();
		
	}
	uint16_t read(ADC_channel channel){
		// reads ADCn, where n is pin
		// select channel, initiate conversion, wait, returns value
		// WARNING: delay may be necessary between setting mux and starting conversion
		ADMUX = (ADMUX & ~(0b11111 << MUX0)) | ((channel & 0x1F) << MUX0);
		ADCSRB = (ADCSRB & ~(0b1 << MUX5)) | ((channel >> 5) << MUX5);
		
		// delay may be necessary if input is high impedance source
		// eg. ADC_BANDGAP
		// 	for(int i = 0; i < 100; i++)
		// 		asm("nop");
		
		ADCSRA |= (0b1 << ADSC); // start conversion
		while(ADCSRA & (0b1 << ADSC)); // wait for conversion to end
		return ADC;
	}
	// configuration
	void enable_adc(){
		ADCSRA |= (0b1 << ADEN);
	}
	void disable_adc(){
		ADCSRA &= ~(0b1 << ADEN);
	}
	void set_prescaler(ADC_prescaler prescaler){
		ADCSRA = (ADCSRA & ~(0b111 << ADPS0)) | (prescaler << ADPS0);
	}
	void set_reference(ADC_voltage_reference vref_sel){
		ADMUX = (ADMUX & ~(0b11 << REFS0)) | (vref_sel << REFS0);
	}
	void set_adjust(ADC_adjust adjust){
		ADMUX = (ADMUX & ~(0b1 << ADLAR)) | (adjust << ADLAR);
	}
	void enable_digital_input(uint16_t mask){
		mask &= DIDR_MASK;
		DIDR2 &= ~(mask >> 8);
		DIDR0 &= ~(mask & 0xFF);
	}
	void disable_digital_input(uint16_t mask){
		mask &= DIDR_MASK;
		DIDR2 |= (mask >> 8);
		DIDR0 |= (mask & 0xFF);
	}
	void enable_high_speed(){
		ADCSRB |= (0b1 << ADHSM);
	}
	void disable_high_speed(){
		ADCSRB &= ~(0b1 << ADHSM);
	}
	// interrupts
	void enable_int(){
		ADCSRA |= (0b1 << ADIE);
	}
	void disable_int(){
		ADCSRA &= ~(0b1 << ADIE);
	}
	bool get_int_flag(){
		return ADCSRA & (0b1 << ADIF);
	}
	void clear_int_flag(){
		ADCSRA |= (0b1 << ADIF);
	}
};

