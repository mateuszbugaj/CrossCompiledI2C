#pragma once

#include <string>
#include <thread>
#include <queue>
#include <iostream>
#include <fstream>

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