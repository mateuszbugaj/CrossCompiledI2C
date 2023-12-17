#include <Device.hpp>

std::string Device::getName(){
  return name;
}

void Device::send(std::string instruction){
  instructions.push(instruction);
}

bool Device::isTransmissionRunning(){
  return transmissionRunning;
}