#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include <I2C.h>
#include <I2C_HAL.h>
#include "USART.h"

#define MASTER_ADDR 51
#define SLAVE_ADDR 52

int main(void) {
  clock_prescale_set(clock_div_1);
  usart_init();

  HAL_Pin sclOutPin, sdaOutPin, sclInPin, sdaInPin;

  I2C_Config i2c_config = {
    .loggingLevel = 4,
    .sclOutPin = HAL_pinSetup(&sclOutPin, &PORTD, 7, PULLUP_ENABLE),
    .sdaOutPin = HAL_pinSetup(&sdaOutPin, &PORTD, 6, PULLUP_ENABLE),
    .sclInPin = HAL_pinSetup(&sclInPin, &PORTD, 5, PULLUP_ENABLE),
    .sdaInPin = HAL_pinSetup(&sdaInPin, &PORTB, 7, PULLUP_ENABLE),
    .timeUnit = 200,
    .print_str = &usart_print,
    .print_num = &usart_print_num,
  };

  HAL_Pin rolePin = {&PORTB, 2, PULLUP_ENABLE};
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

  while (1) {
    if(i2c_config.role == SLAVE){
      I2C_read();
    }

    if(i2c_config.role == MASTER){
      I2C_sendStartCondition();
      I2C_write(52);
      I2C_write(18);
      I2C_sendStopCondition();
      return;
    }
  }

}