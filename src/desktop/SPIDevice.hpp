#pragma once

#include <Device.hpp>

extern "C" {
  #include <SPI.h>
  #include <SPI_HAL.h>
  #include <SPI_HAL_DESKTOP.h>
}

#define PIN_STATE_FILE "output/pins.log"
#define TRANSMITTER_LOG_FILE "output/master.log"
#define RECEIVER_LOG_FILE "output/slave.log"

class SPIDevice : public Device {
  private:
    SPI_Config SPI_config;

  public:
    SPIDevice(std::string name, SPI_Role role);
    ~SPIDevice();
    void executionThreadFunction();
    SPI_Config* getConfig();
};