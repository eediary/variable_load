#include "HAL_UART.h"

// initialize static members
// HAL_UART static members are used to share data with ISR
volatile const char *HAL_UART::tx_char_ptr = NULL;
volatile bool HAL_UART::tx_busy = false;
RBuffer<char, UART_RX_BUFFER_SIZE> *HAL_UART::RX_Buffer_Ptr = NULL;

// ISR
ISR(USART1_UDRE_vect){
	// transmits null-terminated string
	// one byte sent per interrupt
	// interrupt is masked and nothing is sent at null
	if(*HAL_UART::tx_char_ptr)
	UDR1 = *(HAL_UART::tx_char_ptr++);
	else{
		UCSR1B &= ~(0b1 << UDRIE1);
		HAL_UART::tx_busy = false;
	}
}

ISR(USART1_RX_vect){
	HAL_UART::RX_Buffer_Ptr->push(UDR1);
}
