#pragma once

#include "HAL_GPIO.h"
#include "HAL_SPI.h"

#define MAX5216_RESOLUTION (16)
#define MAX5216_SPI_MODE (HAL_SPI::SPI_MODE_1)
#define MAX5216_SPI_CLK_SEL (HAL_SPI::SPI_DIV2) // Max SPI CLK freq = 50 MHz

class MAX5216{
public:
enum MAX5216_control{
	MAX5216_NOP = 0b00,
	MAX5216_POWER_DOWN = 0b10,
	MAX5216_WRITE_THROUGH = 0b01
};
enum MAX5216_POWER_DOWN_MODES{
	MAX5216_RESUME = 0b00,
	OUT_HIGH_Z = 0b01,
	OUT_GND_100K = 0b10,
	OUT_GND_1K = 0b11
};

private:
HAL_GPIO &cs_gpio;
HAL_SPI &spi;
const int resolution;
const float reference_volt;

public:
MAX5216(HAL_GPIO &gpio_r, HAL_SPI &spi_r, float ref_volt) : 
	cs_gpio(gpio_r),
	spi(spi_r),
	resolution(MAX5216_RESOLUTION),
	reference_volt(ref_volt)
{
	// Chip start out disabled, output high impedance
	power_down(OUT_HIGH_Z);
	
}
void nop(){
	// D23 and D22 should be 0b00
	cs_gpio.clear_pin();
	spi.send_tbyte(0, MAX5216_SPI_MODE, MAX5216_SPI_CLK_SEL);
	cs_gpio.set_pin();
}
void power_down(MAX5216_POWER_DOWN_MODES mode){
	// D23 to D21 should be 0b100
	// D19 and D18 should be A1 and A0
	// Rest don't matter
	uint32_t to_send = ((uint32_t) 0b100 << 21) | ((uint32_t) mode << 18);
	cs_gpio.clear_pin();
	spi.send_tbyte(to_send, MAX5216_SPI_MODE, MAX5216_SPI_CLK_SEL);
	cs_gpio.set_pin();
}
void set_output(float output){
	// takes desired output voltage and writes to device
	// D23 and D22 should be 0b01
	// D21 to D6 is 16 bit value
	// Rest don't matter
	uint32_t to_send = (output / reference_volt * ((1UL << resolution)-1));
	to_send = ((to_send & 0xFFFF) << 6) | ((uint32_t) MAX5216_WRITE_THROUGH << 22);
	
	cs_gpio.clear_pin();
	spi.send_tbyte(to_send, MAX5216_SPI_MODE, MAX5216_SPI_CLK_SEL);
	cs_gpio.set_pin();
}
};