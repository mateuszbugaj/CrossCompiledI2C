#pragma once

#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

void USART_init(void);
void USART_print(char content[]);
void USART_printNum(uint16_t number);