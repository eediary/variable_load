#pragma once

#include <avr/io.h>

class HAL_SPI{
public:
enum SPI_MODE{
	SPI_SLAVE = 0b0, SPI_MASTER = 0b1
};
enum SPI_DATA_MODE{
	// CPOL, CPHA
	SPI_MODE_0 = 0b00, SPI_MODE_1 = 0b01, SPI_MODE_2 = 0b10, SPI_MODE_3 = 0b11
};
enum SPI_BIT_ORDER{
	SPI_MSB_FIRST = 0b0, SPI_LSB_FIRST = 0b1
};
enum SPI_CLK_SEL{
	SPI_DIV4 = 0b000, SPI_DIV16 = 0b001, SPI_DIV64 = 0b010, SPI_DIV128 = 0b011, 
	SPI_DIV2 = 0b100, SPI_DIV8 = 0b101, SPI_DIV32 = 0b110, //DIV64 = 0b111 // two ways to set div 64
};
enum SPI_SS_DIR{
	SPI_INPUT = 0b0, SPI_OUTPUT = 0b1
};
private:
void set_data_mode(SPI_DATA_MODE mode){
	switch(mode){
		case SPI_MODE_0:
		SPCR &= ~((0b1 << CPOL) | (0b1 << CPHA));
		break;
		case SPI_MODE_1:
		SPCR = (SPCR & ~(0b1 << CPOL)) | (0b1 << CPHA);
		break;
		case SPI_MODE_2:
		SPCR = (SPCR & ~(0b1 << CPHA)) | (0b1 << CPOL);
		break;
		case SPI_MODE_3:
		SPCR |= (0b1 << CPHA) | (0b1 << CPOL);
		break;
	}
}
public:
HAL_SPI(SPI_MODE mode, SPI_CLK_SEL clk_sel, SPI_BIT_ORDER bit_order = SPI_MSB_FIRST, SPI_SS_DIR ss_dir = SPI_INPUT){
	// constructor
	// make MCU master or slave
	set_spi_mode(mode);
	// Set bit order
	set_bit_order(bit_order);
	// SPI clock speed
	set_clk(clk_sel);
	
	// Sets MOSI (PB2) and SCLK (PB1) as outputs if master
	if(mode == SPI_MASTER)
		DDRB |= ((0b1 << DDB2) | (0b1 << DDB1));
	// Sets SS (PB0) direction
	set_ss_dir(ss_dir);
	// enable SPI
	spi_enable();
}
// configuration
void set_ss_dir(SPI_SS_DIR ss_dir){
	// SS is on PB0
	// WARNING: if SPI is in master mode and SS is input,
	//   SS going low will cause SPI to go into slave mode
	if(ss_dir == SPI_INPUT)
	DDRB &= ~(0b1 << DDB0);
	else
	DDRB |= (0b1 << DDB0);
}
void set_clk(SPI_CLK_SEL clk_sel)
{
	SPSR = (SPSR & ~(0b1 << SPI2X)) | (((clk_sel & 0b100) >> 2) << SPI2X);
	SPCR = (SPCR & ~(0b11 << SPR0)) | ((clk_sel & 0b011) << SPR0);
}

void set_spi_mode(SPI_MODE mode)
{
	SPCR = (SPCR & ~(0b1 << MSTR)) | (mode << MSTR);
}

void spi_enable(){
	SPCR |= (1 << SPE);
}
void spi_disable(){
	SPCR &= ~(1 << SPE);
}
void set_bit_order(SPI_BIT_ORDER order){
	SPCR = (SPCR & ~(0b1 << DORD)) | (order << DORD);
}
// data transmission
uint8_t send_byte(uint8_t data, SPI_DATA_MODE mode){
	// sends byte, waits for transmission to finish, returns byte
	set_data_mode(mode);
	SPDR = data;
	while(!get_int_flag());
	return SPDR;
}
uint8_t send_byte(uint8_t data, SPI_DATA_MODE mode, SPI_CLK_SEL clk_sel){
	set_clk(clk_sel);
	return send_byte(data, mode);
}
uint16_t send_dbyte(uint16_t data, SPI_DATA_MODE mode){
	// sends and returns 16 bits, MSB first
	uint16_t to_return = 0;
	to_return = send_byte(data >> 8, mode);
	to_return <<= 8;
	to_return += send_byte(data & 0xFF, mode);
	return to_return;
}
uint16_t send_dbyte(uint16_t data, SPI_DATA_MODE mode, SPI_CLK_SEL clk_sel){
	set_clk(clk_sel);
	return send_dbyte(data, mode);
}
uint32_t send_tbyte(uint32_t data, SPI_DATA_MODE mode){
	// sends and returns 24 bits, MSB first
	// most significant bit is unused (0x00)
	uint32_t to_return = 0;
	to_return = send_dbyte(data >> 8, mode);
	to_return <<= 8;
	to_return += send_byte(data & 0xFF, mode);
	return to_return;
}
uint32_t send_tbyte(uint32_t data, SPI_DATA_MODE mode, SPI_CLK_SEL clk_sel){
	set_clk(clk_sel);
	return send_tbyte(data, mode);
}
uint32_t send_qbyte(uint32_t data, SPI_DATA_MODE mode){
	// sends and returns 32 bits, MSB first
	uint32_t to_return = 0;
	to_return = send_dbyte(data >> 16, mode);
	to_return <<= 16;
	to_return += send_dbyte(data & 0xFFFF, mode);
	return to_return;
}
uint32_t send_qbyte(uint32_t data, SPI_DATA_MODE mode, SPI_CLK_SEL clk_sel){
	set_clk(clk_sel);
	return send_qbyte(data, mode);
}
// Interrupt
void int_enable(){
	SPCR |= (1 << SPIE);
}
void int_disable(){
	SPCR &= ~(1 << SPIE);
}
// flag
bool get_int_flag(){
	return SPSR & (0b1 << SPIF);
}
bool get_collision_flag(){
	return SPSR & (0b1 << WCOL);
}
};

