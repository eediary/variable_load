#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "RBuffer.h"

#ifndef F_CPU
#define F_CPU 16000000UL
# warning "F_CPU not defined for UART.h"
#endif

#define UART_ASYNCH_MODE (0b00)
#define UART_RX_BUFFER_SIZE (20)
#define UART_DEBUG_BUFFER_SIZE (20)

// Global variables
// Used to pass data between code and ISR
namespace GLOBAL_UART{
	// for transmit interrupt
	volatile const char *tx_char_ptr;
	volatile bool tx_busy = false;
	
	// Receive buffer pointer
	RBuffer <char, UART_RX_BUFFER_SIZE>*RX_Buffer_Ptr;
}

class HAL_UART{
public:
	enum UART_mode{
		UART_MODE_RX = 0b10, UART_MODE_TX = 0b01, UART_MODE_FULL = 0b11, UART_MODE_NONE = 0b00
	};
	enum UART_data_bits{
		UART_5_BITS = 0b000, UART_6_BITS = 0b001, UART_7_BITS = 0b010, UART_8_BITS = 0b011
	};
	enum UART_parity{
		UART_PARITY_NONE = 0b00, UART_PARITY_EVEN = 0b10, UART_PARITY_ODD = 0b11
	};
	enum UART_stop_bits{
		UART_STOP_1 = 0b0, UART_STOP_2 = 0b1
	};
	
private:
	RBuffer <char, UART_RX_BUFFER_SIZE> RX_Buffer;
public:
	HAL_UART(
	unsigned long baud_rate = 9600, 
	UART_mode mode = UART_MODE_FULL,
	UART_data_bits data_len = UART_8_BITS, 
	UART_parity parity = UART_PARITY_NONE, 
	UART_stop_bits num_stop_bits = UART_STOP_1) :
	RX_Buffer()
	{
		// Constructor
		// Default: 9600 baud, TX & RX, 8N1 format
		
		// Set up global pointer to ring buffer
		GLOBAL_UART::RX_Buffer_Ptr = &RX_Buffer;
		// Configure USART for UART
		UCSR1C = (UCSR1C & ~(0b11 << UMSEL10)) | (UART_ASYNCH_MODE << UMSEL10);
		// Configure settings
		set_baud(baud_rate);
		set_data_length(data_len);
		set_parity(parity);
		set_stop_bits(num_stop_bits);
		// Configure pins
		set_mode(mode);
	}
	
	// configuration
	void set_baud(unsigned long baud_rate){
		uint16_t UBRR_val = F_CPU/(16UL * baud_rate) - 1;
		UBRR1H = (UBRR_val >> 8) & (0x0F);
		UBRR1L = UBRR_val & (0xFF);
	}
	void set_mode(UART_mode mode){
		UCSR1B = (UCSR1B & ~(0b11 << TXEN1)) | (mode << TXEN1);
	}
	void set_data_length(UART_data_bits length){
		// Set up USART control register C
		UCSR1C = (UCSR1C & ~(0b11 << UCSZ10)) | ((length & 0b011) << UCSZ10);
		// Set up USART control register B
		UCSR1B &= ~(0b1 << UCSZ12);
	}
	void set_parity(UART_parity parity){
		UCSR1C = (UCSR1C & ~(0b11 << UPM10)) | (parity << UPM10);
	}
	void set_stop_bits(UART_stop_bits num_bits){
		UCSR1C = (UCSR1C & ~(0b1 << USBS1)) | (num_bits << USBS1);
	}
	
	// basic rx and tx
	void send_byte(uint8_t data){
		// waits until register is ready, then writes
		while(!(UCSR1A & (1 << UDRE1)));
		UDR1 = data;
	}
	bool read_byte(uint8_t &val){
		// writes data to val if available
		// returns false if no data available
		if(is_available()){
			val = UDR1;
			return true;
		} else{
			return false;
		}
	}
	bool is_available(){
		return UCSR1A & (0b1<<RXC1);
	}
	
	// interrupt enable & disable
	void enable_rx_int(){
		UCSR1B |= (0b1 << RXCIE1);
	}
	void disable_rx_int(){
		UCSR1B &= ~(0b1 << RXCIE1);
	}
	void enable_tx_int(){
		UCSR1B |= (0b1 << TXCIE1);
	}
	void disable_tx_int(){
		UCSR1B &= ~(0b1 << TXCIE1);
	}
	void enable_data_reg_int(){
		UCSR1B |= (0b1 << UDRIE1);
	}
	void disable_data_reg_int(){
		UCSR1B &= ~(0b1 << UDRIE1);
	}
	// higher level rx and tx
	void send_string(const char *char_ptr){
		// Waits for TX to be free, then transmits null-terminated string
		while(GLOBAL_UART::tx_busy);
		while(*char_ptr){
			send_byte(*char_ptr);
			char_ptr++;
		}
	}
	void send_string(const char *header, int val, const char *footer){
		char char_buffer[UART_DEBUG_BUFFER_SIZE];
		itoa(val, char_buffer, 10);
		send_string(header);
		send_string(char_buffer);
		send_string(footer);
	}
	void send_string(const char *header, long val, const char *footer){
		char char_buffer[UART_DEBUG_BUFFER_SIZE];
		ltoa(val, char_buffer, 10);
		send_string(header);
		send_string(char_buffer);
		send_string(footer);
	}
	void send_string(const char *header, float val, int precision, const char *footer){
		char char_buffer[UART_DEBUG_BUFFER_SIZE];
		dtostre(val, char_buffer, precision, DTOSTR_ALWAYS_SIGN | DTOSTR_UPPERCASE);
		send_string(header);
		send_string(char_buffer);
		send_string(footer);
	}
	void send_string(const char *header, char *val, const char *footer){
		send_string(header);
		send_string(val);
		send_string(footer);
	}
	bool send_string_int(const char *char_ptr){
		// Transmits null-terminated string using interrupt
		// Returns true if transmission begins, false if transmitter is taken
		// NOTE: requires global and USART Data Register Empty Interrupt enabled
		if(GLOBAL_UART::tx_busy){
			// Transmission ongoing; failure
			return false;
			} else{
			// No transmission ongoing
			GLOBAL_UART::tx_busy = true;
			GLOBAL_UART::tx_char_ptr = (volatile const char*)char_ptr;
			enable_data_reg_int();
			return true;
		}
	}
	int read_rx_buffer(char *buffer, int buffer_size){
		// Copies contents of ring buffer to provided buffer + null termination
		// Copied elements are removed from ring buffer
		// returns number of elements copied to buffer, not including null
		// NOTE: requires global and RX Complete Interrupt enabled
		int num_copy = RX_Buffer.flush(buffer, buffer_size-1);
		buffer[num_copy] = 0;
		return num_copy;
	}
	
	// misc
	void flush_rx(){
		// flushes USART Data Register + receive buffer registers
		uint8_t dummy;
		while(read_byte(dummy));
	}
	int get_rx_buffer_size(){
		return UART_RX_BUFFER_SIZE;
	}
	void clear_overflow(){
		RX_Buffer.clear_overflow();
	}
	bool get_overflow(){
		return RX_Buffer.get_overflow();
	}
	uint16_t set_baud(){
		return ((UBRR1H & 0xF) << 8) | UBRR1L;
	}
};


// ISR
ISR(USART1_UDRE_vect){
	// transmits null-terminated string
	// one byte sent per interrupt
	// interrupt is masked and nothing is sent at null
	if(*GLOBAL_UART::tx_char_ptr)
	UDR1 = *(GLOBAL_UART::tx_char_ptr++);
	else{
		UCSR1B &= ~(0b1 << UDRIE1);
		GLOBAL_UART::tx_busy = false;
	}
}

ISR(USART1_RX_vect){
	GLOBAL_UART::RX_Buffer_Ptr->push(UDR1);
}