#pragma once
#define BAUD 9600

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdbool.h>

#define USART_BUFFER_SIZE 20
#define ASCII_BACKSPACE 127

char usart_last_command[USART_BUFFER_SIZE];
uint8_t usart_last_command_length;

void usart_init(void);
void usart_print(char content[]);
void usart_print_num(uint16_t number);
void usart_print_float(float number, int precision);
void usart_print_char(char c);
bool usart_compare_str(char* s1, char* s2, uint8_t s1_size, uint8_t s2_size);
float* usart_get_numbers(char str[], uint8_t str_size, float* buffer);
bool usart_contains_str(char* s1, char* s2, uint8_t s1_size, uint8_t s2_size);
bool usart_command_processed();
void usart_waitForCommand();