#include <cstddef>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include <I2C_HAL.h>
#include <I2C_HAL_STM32.h>
extern "C" {
  #include <I2C.h>
  #include <console.h>
}
#include "USART.hpp"

#define MASTER_ADDR 82

void I2C_print(I2C_Role role, char str[]){
  USART_print(str);
}

void I2C_printNum(I2C_Role role, uint16_t num){
  USART_printNum(num);
}

int main(void){
  rcc_clock_setup_in_hse_8mhz_out_72mhz();

  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOA);

  USART_init();

  HAL_Pin sclOutPin, sdaOutPin, sclInPin, sdaInPin;

  I2C_Config i2c_config {
    .addr = MASTER_ADDR,
    .loggingLevel = 4,
    .role = I2C_Role::MASTER,
    .sclOutPin = HAL_pinSetup(&sclOutPin, GPIOB, GPIO11),
    .sdaOutPin = HAL_pinSetup(&sdaOutPin, GPIOB, GPIO10),
    .sclInPin = HAL_pinSetup(&sclInPin, GPIOB, GPIO12),
    .sdaInPin = HAL_pinSetup(&sdaInPin, GPIOB, GPIO13),
    .timeUnit = 100,
    .print_str = &I2C_print,
    .print_num = &I2C_printNum
  };

  I2C_init(&i2c_config);
  console_init(&USART_print);

  int count = 0;
  while (1){
    if(i2c_config.role == I2C_Role::SLAVE){
      I2C_read(&i2c_config);
    }

    USART_getc(0);
    if(!USART_commandProcessed()){
      console_parse(&i2c_config, USART_lastCommand);
      USART_print("> ");
    }
  }

  return 0;
}