#include "USART.hpp"

void USART_init(void){
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);

	// UART TX on PA9 (GPIO_USART1_TX)
	gpio_set_mode(GPIOA,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
		GPIO_USART1_TX);

	usart_set_baudrate(USART1,38400);
	usart_set_databits(USART1,8);
	usart_set_stopbits(USART1,USART_STOPBITS_1);
	usart_set_mode(USART1,USART_MODE_TX);
	usart_set_parity(USART1,USART_PARITY_NONE);
	usart_set_flow_control(USART1,USART_FLOWCONTROL_NONE);
	usart_enable(USART1);
}

void USART_print(char content[]){
  for (char *p = content; *p != '\0'; p++) {
    usart_send_blocking(USART1,*p);
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