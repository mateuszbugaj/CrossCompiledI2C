#pragma once

#include <string>
#include <thread>
#include <queue>
#include <iostream>
#include <fstream>

#define PIN_STATE_FILE "output/pins.log"
#define TRANSMITTER_LOG_FILE "output/master.log"
#define RECEIVER_LOG_FILE "output/slave.log"

class Device {
  protected:
    std::string name;
    std::thread executionTread;
    std::queue<std::string> instructions;
    bool transmissionRunning = false;

  public:
    void send(std::string instruction);
    std::string getName();
    bool isTransmissionRunning();
};