#include <cstddef>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include <I2C_HAL.h>
extern "C" {
  #include <I2C.h>
  #include <console.h>
}
#include "USART.hpp"

#define SLAVE_ADDR 82

int main(void){
  rcc_clock_setup_in_hse_8mhz_out_72mhz();

  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOA);
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13); // Output on PC13
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0); // Input on PA0

  bool clicked = false;
  USART_init();

  HAL_Pin sclOutPin, sdaOutPin, sclInPin, sdaInPin;

  uint16_t GPIOB_PORT = GPIOB;

  I2C_Config i2c_config {
    .addr = SLAVE_ADDR,
    .loggingLevel = 4,
    .role = I2C_Role::SLAVE,
    .sclOutPin = HAL_pinSetup(&sclOutPin, &GPIOB_PORT, GPIO10, HAL_PullupConfig::PULLUP_ENABLE),
    .sdaOutPin = HAL_pinSetup(&sdaOutPin, &GPIOB_PORT, GPIO11, HAL_PullupConfig::PULLUP_ENABLE),
    .sclInPin = HAL_pinSetup(&sclInPin, &GPIOB_PORT, GPIO0, HAL_PullupConfig::PULLUP_ENABLE),
    .sdaInPin = HAL_pinSetup(&sdaInPin, &GPIOB_PORT, GPIO1, HAL_PullupConfig::PULLUP_ENABLE),
    .timeUnit = 100,
    .print_str = &USART_print,
    .print_num = &USART_printNum
  };

  I2C_init(&i2c_config);
  console_init(&i2c_config, &USART_print);

  int count = 0;
  while (1){
    if(i2c_config.role == I2C_Role::SLAVE){
      I2C_read();
    }

    USART_getc(0);
    if(!USART_commandProcessed()){
      console_parse(USART_lastCommand);
    }

    if(gpio_get(GPIOA, GPIO0)){
      if(clicked == false){
        gpio_clear(GPIOC, GPIO13);
        clicked = true;
        USART_print("Cliecked: ");
        USART_printNum(count++);
        USART_print("\n\r");
      }
    } else {
      gpio_set(GPIOC, GPIO13);
      clicked = false;
    }
  }

  return 0;
}