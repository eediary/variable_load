#pragma once

#include <avr/io.h>

/*
* SCL frequency = F_CPU / (16 + 2 * TWBR * 4 ^ (TWPS))
* F_CPU = system clock frequency
* TWBR = value of TWI Bit rate register
* TWPS = value of prescaler bits in TWI status register
* TWBR should be greater than or equal to 10
* At F_CPU = 16 MHz:
*    SCL frequency ~ 1   kHz, TWBR = 125, TWPS = 3 (DIV64)
*    SCL frequency = 10  kHz, TWBR = 198, TWPS = 1 (DIV4)
*    SCL frequency = 50  kHz, TWPS = 152, TWPS = 0 (DIV1)
*    SCL frequency = 100 kHz, TWPS = 72,  TWPS = 0 (DIV1)
*    SCL frequency = 200 kHz, TWPS = 32,  TWPS = 0 (DIV1)
*    SCL frequency = 400 kHz, TWPS = 12,  TWPS = 0 (DIV1)
*/
class HAL_TWI{
private:
uint16_t timeout_limit;
public:
enum TWI_STATUS{
	// Master Transmitter
	TWI_START_TRANSMITTED = 0x08,
	TWI_REPEATED_START_TRANSMITTED = 0x10,
	TWI_ADDR_WRITE_TRANSMITTED_ACK = 0x18,
	TWI_ADDR_WRITE_TRANSMITTED_NAK = 0x20,
	TWI_DATA_TRANSMITTED_ACK = 0x28,
	TWI_DATA_TRANSMITTED_NAK = 0x30,
	TWI_ARBITRATION_LOST = 0x38,
	// Master Receiver
	TWI_ADDR_READ_TRANSMITTED_ACK = 0x40,
	TWI_ADDR_READ_TRANSMITTED_NAK = 0x48,
	TWI_DATA_RECEIVED_ACK = 0x50,
	TWI_DATA_RECEIVED_NAK = 0x58,
	// Misc
	TWI_NO_INFO = 0xF8,
	TWI_ILLEGAL_STOP_STOP = 0x00
};
enum TWI_ERROR{
	TWI_NO_ERROR = 0,
	TWI_ADDRESS_NAK = 1,
	TWI_DATA_NAK = 2,
	TWI_NO_START = 3,
	TWI_TIME_OUT = 4,
	TWI_MISC = 5
};
enum TWI_DIRECTION{
	TWI_WRITE = 0b0,
	TWI_READ = 0b1
};
enum TWI_PRESCALER{
	TWI_DIV1 = 0b00,
	TWI_DIV4 = 0b01,
	TWI_DIV16 = 0b10,
	TWI_DIV64 = 0b11
};
public:
HAL_TWI(
	uint8_t bitrate = 72, TWI_PRESCALER prescaler = TWI_DIV1, 
	bool output_ACK = true, uint8_t addr = 0, 
	bool gen_call_ACK = 1, uint8_t addr_mask = 0, uint16_t timeout = 0)
{
	// Constructor:
	// SCL = 100 kHz by default
	set_bitrate(bitrate);
	set_prescaler(prescaler);
	if(output_ACK)
		enable_ACK();
	else
		disable_ACK();
	set_address(addr, gen_call_ACK);
	set_address_mask(addr_mask);
	set_timeout(timeout);
	enable();
}
void enable(){
	TWCR |= (0b1 << TWEN);
}
void disable(){
	TWCR &= ~(0b1 << TWEN);
}
void set_bitrate(uint8_t val){
	TWBR = val;
}
void set_prescaler(TWI_PRESCALER prescaler){
	TWSR = (TWSR & ~(0b11)) | prescaler;
}
void enable_ACK(){
	TWCR |= (0b1 << TWEA);
}
void disable_ACK(){
	TWCR &= ~(0b1 << TWEA);
}
bool get_collision_flag(){
	return TWCR & (0b1 << TWWC);
}
void enable_int(){
	TWCR |= (0b1 << TWIE);
}
void disable_int(){
	TWCR &= ~(0b1 << TWIE);
}
void set_address(uint8_t val, bool gen_call_ACK){
	// Sets I2C slave address & whether general call is recognized or not
	TWAR = (val & 0xFE);
	if(gen_call_ACK)
		TWAR |= 0b1;
}
void set_address_mask(uint8_t val){
	TWAMR = (val & 0xFE);
}
TWI_ERROR start(){
	// Set TWI to send start
	// Wait until start is transmitted
	TWCR = (TWCR | (0b1 << TWINT) | (0b1 << TWSTA) | (0b1 << TWEN)) & ~(0b1 << TWSTO);
	wait_for_twi();
	TWI_STATUS status = get_status();
	if((status == TWI_START_TRANSMITTED) || (status == TWI_REPEATED_START_TRANSMITTED))
		return TWI_NO_ERROR;
	else if(status == TWI_NO_INFO)
		return TWI_TIME_OUT;
	else
		return TWI_MISC;
}
TWI_ERROR address(uint8_t addr, TWI_DIRECTION dir){
	// Checks if START has been sent; fails if it hasn't
	// loads 7 MSb of addr into TWDR
	// LSb of TWDR is 1 if reading, 0 if writing
	// Returns NO_ERROR if address is ACK, ADDRESS_NAK if NAK, ERROR for misc.
	if((get_status() == TWI_START_TRANSMITTED) || (get_status() == TWI_REPEATED_START_TRANSMITTED)){
		TWDR = (addr & 0xFE) | (dir);
		TWCR = (TWCR | (0b1 << TWINT) | (0b1 << TWEN)) & ~((0b1 << TWSTA) | (0b1 << TWSTO));
		wait_for_twi();
		TWI_STATUS status = get_status();
		if((status == TWI_ADDR_WRITE_TRANSMITTED_ACK) || (status == TWI_ADDR_READ_TRANSMITTED_ACK))
			// Address is ACK
			return TWI_NO_ERROR;
		else if ((status == TWI_ADDR_WRITE_TRANSMITTED_NAK) || (status == TWI_ADDR_READ_TRANSMITTED_NAK))
			// Address is NAK
			return TWI_ADDRESS_NAK;
		else if(status == TWI_NO_INFO)
			// Timeout
			return TWI_TIME_OUT;
		else
			// Something went wrong
			return TWI_MISC;
	} else{
		// Improper setup; fails
		return TWI_NO_START;
	}
}
TWI_ERROR send_data(uint8_t val){
	// Transmits val if address is ACK by receiver, fails otherwise
	if((get_status() == TWI_ADDR_WRITE_TRANSMITTED_ACK) || (get_status() == TWI_DATA_TRANSMITTED_ACK)){
		TWDR = val;
		TWCR = (TWCR | (0b1 << TWINT) | (0b1 << TWEN)) & ~((0b1 << TWSTA) | (0b1 << TWSTO));
		wait_for_twi();
		TWI_STATUS status = get_status();
		if(status == TWI_DATA_TRANSMITTED_ACK)
			return TWI_NO_ERROR;
		else if(status == TWI_DATA_TRANSMITTED_NAK)
			// data was not acknowledged
			return TWI_DATA_NAK;
		else if(status == TWI_NO_INFO)
			// Timeout
			return TWI_TIME_OUT;
		else
			// something went wrong
			return TWI_MISC;
	} else{
		// Cannot transmit
		if(get_status() == TWI_ADDR_WRITE_TRANSMITTED_NAK)
			// Address is NAK; fails
			return TWI_ADDRESS_NAK;
		else if(get_status() == TWI_DATA_TRANSMITTED_ACK)
			// Data is NAK; fails
			return TWI_DATA_NAK;
		else
			// Something went wrong
			return TWI_MISC;
	}
}
TWI_ERROR read_data(uint8_t &val, bool gen_ACK){
	// Reads data to val if ACK is received; fails otherwise
	if((get_status() == TWI_DATA_RECEIVED_ACK) || (get_status() == TWI_ADDR_READ_TRANSMITTED_ACK)){
		// get data
		if(gen_ACK)
			// generate ACK
			TWCR = (TWCR | (0b1 << TWINT) | (0b1 << TWEA) | (0b1 << TWEN)) & ~((0b1 << TWSTA) | (0b1 << TWSTO));
		else
			// don't ACK; ends data transmission
			TWCR = (TWCR | (0b1 << TWINT) | (0b1 << TWEN)) & ~((0b1 << TWEA) | (0b1 << TWSTA) | (0b1 << TWSTO));
		wait_for_twi();
		TWI_STATUS status = get_status();
		// check status & return
		if((status == TWI_DATA_RECEIVED_ACK) || (status == TWI_DATA_RECEIVED_NAK)){
			// Data received
			// NAK isn't a problem here
			val = TWDR;
			return TWI_NO_ERROR;
		} else if(get_status() == TWI_NO_INFO)
			// Timeout
			return TWI_TIME_OUT;
		else
			return TWI_MISC;
	} else if(get_status() == TWI_ADDR_READ_TRANSMITTED_NAK)
		return TWI_ADDRESS_NAK;
	else if(get_status() == TWI_DATA_RECEIVED_NAK)
		return TWI_DATA_NAK;
	else
		return TWI_MISC;
}
TWI_ERROR stop(){
	// Transmits stop
	// Returns error
	TWI_ERROR to_return = TWI_NO_ERROR;
	TWI_STATUS status = get_status();
	if((status == TWI_ADDR_READ_TRANSMITTED_NAK) || (status == TWI_ADDR_WRITE_TRANSMITTED_NAK))
		// Address not acknowledged
		to_return = TWI_ADDRESS_NAK;
	else if(status == TWI_DATA_TRANSMITTED_NAK)
		// Data not acknowledged
		to_return = TWI_DATA_NAK;
	else if(status == TWI_NO_INFO)
		// Timeout occurred
		to_return = TWI_TIME_OUT;
	else if((status == TWI_START_TRANSMITTED) || (status == TWI_REPEATED_START_TRANSMITTED) 
		|| (status == TWI_ADDR_READ_TRANSMITTED_ACK) || (status == TWI_ADDR_WRITE_TRANSMITTED_ACK)
		|| (status == TWI_DATA_RECEIVED_ACK) || (status == TWI_DATA_RECEIVED_NAK)
		|| (status == TWI_DATA_TRANSMITTED_ACK)
		)
		// No problem
		to_return = TWI_NO_ERROR;
	else
		// Something went wrong
		to_return = TWI_MISC;
	TWCR = (TWCR | (0b1 << TWINT) | (0b1 << TWSTO) | (0b1 << TWEN)) & ~(0b1 << TWSTA);
	return to_return;
}
void stop_start(){
	TWCR = (TWCR | (0b1 << TWINT) | (0b1 << TWSTA) | (0b1 << TWSTO) | (0b1 << TWEN));
}
TWI_STATUS get_status(){
	// Get TWS7 to TWS3
	return (TWI_STATUS) (TWSR & (0xF8));
}
void wait_for_twi(){
	// waits for TWINT in TWCR to become set
	// returns failure if illegal operation occurred
	// can input number to set timeout limit
	// WARNING: improper use of TWI may cause this loop to last forever
	//   eg. improper termination of read request by ACKing last byte
	uint16_t counter = 0;
	while(!(TWCR & (0b1 << TWINT))){
		if(get_status() == TWI_ILLEGAL_STOP_STOP)
			// error has occurred
			return;
		if(timeout_limit && ((counter++) >= timeout_limit))
			// time-out has occurred
			return;
	}
	// No error
	return;
}
void set_timeout(uint16_t val){
	timeout_limit = val;
}
uint16_t get_timeout(){
	return timeout_limit;
}
};