// https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f4/stm32f429i-discovery/usart_console/clock.c

#include "USART.hpp"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#define CONSOLE_UART	USART1

/* milliseconds since boot */
static volatile uint32_t system_millis;
char USART_lastCommand[USART_BUFFER_SIZE];
volatile uint16_t usart_index = 0;
volatile bool commandProcessed = true;

/* Called when systick fires */
void sys_tick_handler(void){
	system_millis++;
}

/* simple sleep for delay milliseconds */
void msleep(uint32_t delay){
	uint32_t wake = system_millis + delay;
	while (wake > system_millis);
}

/* Getter function for the current time */
uint32_t mtime(void){
	return system_millis;
}

void clock_setup(void){
	/* clock rate / 72000 to get 1mS interrupt rate */
	systick_set_reload(72000);
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();

	systick_interrupt_enable();
}

void USART_init(void){
	clock_setup();

	rcc_periph_clock_enable(RCC_GPIOA);

  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO10);

	rcc_periph_clock_enable(RCC_USART1);

	usart_set_baudrate(CONSOLE_UART, 38400);
	usart_set_databits(CONSOLE_UART, 8);
	usart_set_stopbits(CONSOLE_UART, USART_STOPBITS_1);
	usart_set_mode(CONSOLE_UART, USART_MODE_TX_RX);
	usart_set_parity(CONSOLE_UART, USART_PARITY_NONE);
	usart_set_flow_control(CONSOLE_UART, USART_FLOWCONTROL_NONE);
	usart_enable(CONSOLE_UART);
}

void transmit_byte(char c){
	uint32_t	reg;
	do {
		reg = USART_SR(CONSOLE_UART);
	} while ((reg & USART_SR_TXE) == 0);
	USART_DR(CONSOLE_UART) = (uint16_t) c & 0xff;
}

void USART_print(char* s){
	while (*s != '\0') {
		transmit_byte(*s);
		s++;
	}
}

void USART_getc(int wait){

	uint32_t	reg;
	reg = USART_SR(CONSOLE_UART);
	if((reg & USART_SR_RXNE) != 0){
		uint8_t byte = USART_DR(CONSOLE_UART);

    if(byte == 127){ // backspace 
        USART_print("\b \b");
        USART_lastCommand[usart_index] = '\0';
        if(usart_index > 0) usart_index--;
        return;
    }

    if(byte!='\r')
        transmit_byte(byte);

    if(byte == '\r'){
        USART_print("\r\n");

        USART_lastCommand[usart_index] = '\0';

        usart_index = 0;
        commandProcessed = false;
    } else {
        if(usart_index < USART_BUFFER_SIZE - 1) {
            USART_lastCommand[usart_index] = byte;
            usart_index++;
        }
    }
	}
}

void USART_printNum(uint16_t number){
	char buffer[6] {};
	bool isNegative = number < 0;
	uint8_t index = 0;

	if(number == 0) {
		USART_print("0");
		return;
	}

	if(isNegative){
		number *= -1;
	}

	while(number > 0){
		buffer[index] = (number % 10) + '0';
		number /= 10;
		index++;
	}

	// reverse the string in the buffer
	for(uint8_t i = 0; i < index/2; i++){
		char temp = buffer[i];
		buffer[i] = buffer[index - i - 1];
		buffer[index - i - 1] = temp;
	}

	if(isNegative){
		for(uint8_t i = index; i > 0; i--){
			buffer[i] = buffer[i - 1];
		}

		buffer[0] = '-';
		index++;
	}

	buffer[index] = '\0';

	USART_print(buffer);
}

bool USART_commandProcessed(){
    if(!commandProcessed){
        commandProcessed = true;
        return false;
    }

    return true;
}