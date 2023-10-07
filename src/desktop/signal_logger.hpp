#pragma once
#include <vector>
extern "C" {
  #include <I2C_HAL.h>
}

#define OUTPUT_FILE "output/pin_states.log"
void SL_logPinState(HAL_Pin* signals, uint8_t size);