#pragma once

#include "HAL_TWI.h"
#define PCF8574_SCL_PRESCALER (HAL_TWI::TWI_DIV1) // Max I2C clock speed = 100 kHz
#define PCF8574_SCL_BITRATE (72)
class PCF8574{
public:
enum PCF8574_ADDRESS{
	// Address determined by A2, A1 and A0
	ADDR_000 = 0b01000000,
	ADDR_001 = 0b01000010,
	ADDR_010 = 0b01000100,
	ADDR_011 = 0b01000110,
	ADDR_100 = 0b01001000,
	ADDR_101 = 0b01001010,
	ADDR_110 = 0b01001100,
	ADDR_111 = 0b01001110
};

private:
HAL_TWI &twi;
PCF8574_ADDRESS addr;
public:
PCF8574(HAL_TWI &twi_r, PCF8574_ADDRESS input_addr): 
	twi(twi_r),
	addr(input_addr)
{
	// Constructor
	// Set up twi and addr
}
HAL_TWI::TWI_ERROR read(uint8_t &val){
	// reads from device
	// returns error code, updates val
	twi.set_prescaler(PCF8574_SCL_PRESCALER);
	twi.set_bitrate(PCF8574_SCL_BITRATE);
	HAL_TWI::TWI_ERROR error;
	if((error = twi.start()) != HAL_TWI::TWI_NO_ERROR)
		return error;
	if((error = twi.address(addr, HAL_TWI::TWI_READ)) != HAL_TWI::TWI_NO_ERROR)
		return error;
	if((error = twi.read_data(val, false)) != HAL_TWI::TWI_NO_ERROR)
		return error;
	return twi.stop();
}
HAL_TWI::TWI_ERROR write(uint8_t val){
	// writes to device
	// returns error code
	twi.set_prescaler(PCF8574_SCL_PRESCALER);
	twi.set_bitrate(PCF8574_SCL_BITRATE);
	HAL_TWI::TWI_ERROR error;
	if((error = twi.start()) != HAL_TWI::TWI_NO_ERROR)
		return error;
	if((error = twi.address(addr, HAL_TWI::TWI_WRITE)) != HAL_TWI::TWI_NO_ERROR)
		return error;
	if((error = twi.send_data(val)) != HAL_TWI::TWI_NO_ERROR)
		return error;
	return twi.stop();
}
};