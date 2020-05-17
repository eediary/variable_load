#include "HAL_UART.h"
namespace GLOBAL_UART{
	// for transmit interrupt
	volatile const char *tx_char_ptr;
	volatile bool tx_busy = false;
	
	// Receive buffer pointer
	RBuffer <char, UART_RX_BUFFER_SIZE>*RX_Buffer_Ptr;
}

HAL_UART::HAL_UART(
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
void HAL_UART::send_string(const char *char_ptr){
	// Waits for TX to be free, then transmits null-terminated string
	while(GLOBAL_UART::tx_busy);
	while(*char_ptr){
		send_byte(*char_ptr);
		char_ptr++;
	}
}
bool HAL_UART::send_string_int(const char *char_ptr){
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
