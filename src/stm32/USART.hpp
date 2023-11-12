#pragma once

#include <stdint.h>

#define USART_BUFFER_SIZE 20

extern char USART_lastCommand[USART_BUFFER_SIZE];

void USART_init(void);
void USART_print(char* s);
void USART_printNum(uint16_t number);
void USART_getc(int wait);
bool USART_commandProcessed();