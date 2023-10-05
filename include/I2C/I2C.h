#pragma once
#include <stdint.h>
#include <HAL/HAL.h>

typedef struct  {
  uint8_t addr;
  void (*print_str)(char[]);
  void (*print_num)(uint8_t);
  uint8_t loggingLevel;
  HAL_Pin* sclPin;
} I2C_config;

void I2C_setup(I2C_config* config);
