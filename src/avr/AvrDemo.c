#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include <I2C.h>
#include <I2C_HAL.h>
#include <I2C_HAL_AVR.h>
#include "USART.h"
#include <console.h>

#define MASTER_ADDR 51
#define SLAVE_ADDR 52

void I2C_print(I2C_Role role, char str[]){
  usart_print(str);
}

void I2C_printNum(I2C_Role role, uint16_t num){
  usart_print_num(num);
}

int main(void) {
  clock_prescale_set(clock_div_1);
  usart_init();

  HAL_Pin sclOutPin, sdaOutPin, sclInPin, sdaInPin;

  I2C_Config i2c_config = {
    .loggingLevel = 4,
    .sclOutPin = HAL_pinSetup(&sclOutPin, &PORTD, &PIND, 7, &DDRD),
    .sdaOutPin = HAL_pinSetup(&sdaOutPin, &PORTD, &PIND, 6, &DDRD),
    .sclInPin = HAL_pinSetup(&sclInPin, &PORTD, &PIND, 5, &DDRD),
    .sdaInPin = HAL_pinSetup(&sdaInPin, &PORTB, &PINB, 7, &DDRB),
    .timeUnit = 200,
    .print_str = &I2C_print,
    .print_num = &I2C_printNum,
  };

  HAL_Pin rolePin;
  HAL_pinSetup(&rolePin, &PORTB, &PINB, 2, &DDRB);
  HAL_setPinDirection(&rolePin, INPUT);

  if(HAL_pinRead(&rolePin) == LOW){
    i2c_config.role = MASTER;
    i2c_config.addr = MASTER_ADDR;
    _delay_ms(500);
  } else {
    i2c_config.role = SLAVE;
    i2c_config.addr = SLAVE_ADDR;
  }

  I2C_init(&i2c_config);
  console_init(&usart_print);

  while (1) {
    if(i2c_config.role == SLAVE){
      I2C_read(&i2c_config);
      if(!usart_command_processed()){
        console_parse(&i2c_config, usart_last_command);
        usart_print("> ");
      }
    } else {
      if(!usart_command_processed()){
        console_parse(&i2c_config, usart_last_command);
        usart_print("> ");
      }
    }
  }

}