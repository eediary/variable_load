#pragma once

#include "HAL_TWI.h"

#define LTC2451_ADDRESS (0b00101000)
#define LTC2451_RESOLUTION (16)
#define LTC2451_SCL_PRESCALER (HAL_TWI::TWI_DIV1) // Max I2C clock speed = 400 kHz
#define LTC2451_SCL_BITRATE (12)
class LTC2451{
public:
enum LTC2451_SPEED{
	SPEED_60HZ = 0b0,
	SPEED_30HZ = 0b1
};

private:
const int resolution;
const float voltage_reference;
HAL_TWI &twi;

public:
LTC2451(HAL_TWI &twi_r, float volt_ref): resolution(LTC2451_RESOLUTION), voltage_reference(volt_ref), twi(twi_r){	
	// Constructor
	// Sets up resolution, voltage reference and twi
}
HAL_TWI::TWI_ERROR set_speed(LTC2451_SPEED speed){
	// sets speed of device
	// returns error code
	twi.set_prescaler(LTC2451_SCL_PRESCALER);
	twi.set_bitrate(LTC2451_SCL_BITRATE);
	HAL_TWI::TWI_ERROR error;
	if((error = twi.start()) != HAL_TWI::TWI_NO_ERROR)
		return error;
	if((error = twi.address(LTC2451_ADDRESS, HAL_TWI::TWI_WRITE)) != HAL_TWI::TWI_NO_ERROR)
		return error;
	if((error = twi.send_data(speed)) != HAL_TWI::TWI_NO_ERROR)
		return error;
	return twi.stop();
}
HAL_TWI::TWI_ERROR read(float &val){
	// reads voltage from device
	// writes result to provided reference
	// return error code
	uint8_t temp_val1, temp_val2;
	twi.set_prescaler(LTC2451_SCL_PRESCALER);
	twi.set_bitrate(LTC2451_SCL_BITRATE);
	HAL_TWI::TWI_ERROR error;
	if((error = twi.start()) != HAL_TWI::TWI_NO_ERROR)
		return error;
	if((error = twi.address(LTC2451_ADDRESS, HAL_TWI::TWI_READ)) != HAL_TWI::TWI_NO_ERROR)
		return error;
	if((error = twi.read_data(temp_val1, true)) != HAL_TWI::TWI_NO_ERROR)
		return error;
	if((error = twi.read_data(temp_val2, false)) != HAL_TWI::TWI_NO_ERROR)
		return error;
	if((error = twi.stop()) != HAL_TWI::TWI_NO_ERROR)
		return error;
	val = ((temp_val1 << 8) + temp_val2) * voltage_reference / ((1UL << resolution)-1);
	return error;
}

};