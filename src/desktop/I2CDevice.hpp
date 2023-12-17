#pragma once

#include <Device.hpp>

extern "C" {
  #include <I2C_HAL.h>
  #include <I2C_HAL_DESKTOP.h>
  #include <I2C.h>
  #include <console.h>
}

#define PIN_STATE_FILE "output/pins.log"
#define TRANSMITTER_LOG_FILE "output/master.log"
#define RECEIVER_LOG_FILE "output/slave.log"

class I2CDevice : public Device {
  private:
    I2C_Config I2C_config;

  public:
    I2CDevice(std::string name, uint8_t addreess, I2C_Role role);
    ~I2CDevice();
    I2C_Config getConfig();
    void executionThreadFunction();
};