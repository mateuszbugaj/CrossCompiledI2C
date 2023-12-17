#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <queue>
#include <string.h>
#include <fstream>

#include <I2CDevice.hpp>
#include <SPIDevice.hpp>

extern "C" {
  #include <I2C_HAL.h>
  #include <I2C_HAL_DESKTOP.h>
  #include <I2C.h>
  #include <SPI_HAL.h>
  #include <SPI_HAL_DESKTOP.h>
  #include <SPI.h>
  #include <console.h>
}

#define TRANSMITTER_ADDRESS 51
#define RECEIVER_ADDRESS 52

typedef enum {
  I2C,
  SPI
} Protocol;

void consolePrint(char str[]){
  std::cout << str;
}

void clearFile(std::string file){
  std::ofstream ofs(file, std::ios::trunc);
  ofs.close();
}

void userInputThreadFunction(Device& transmitter, Device& receiver){
  Device* device = &transmitter;

  printf("%s: ", device->getName().c_str());
  while(true){
    std::string input;
    std::getline(std::cin, input);

    if(input == "t") device = &transmitter;
    else if(input == "r") device = &receiver;
    else if(!input.empty()) device->send(input);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    printf("%s: ", device->getName().c_str());
  }
}

void printHelp(){
  std::cout << "Arguments:\n";
  std::cout << "--protocol [I2C|SPI]\n";
  std::cout << "--help\n";
}

int main(int argc, char** argv){
  Protocol protocol = Protocol::I2C;
  for(int i = 0; i < argc; i++){
    std::string arg(argv[i]);
    if(arg == "--protocol"){
      if(i + 1 > argc){
        std::cout << "Bad arguments\n";
        printHelp();
        return 0;
      }

      std::string protocolStr(argv[i+1]);
      if(protocolStr == "I2C"){
        protocol = Protocol::I2C;
      } else if(protocolStr == "SPI"){
        protocol = Protocol::SPI;
      } else {
        std::cout << "Bad arguments\n";
        printHelp();
        return 0;
      }
    }

    if(arg == "--help"){
      printHelp();
      return 0;
    }
  }

  std::cout << "Protocol: " << (protocol == Protocol::I2C ? "I2C" : "SPI") << std::endl;

  clearFile(TRANSMITTER_LOG_FILE);
  clearFile(RECEIVER_LOG_FILE);
  clearFile(PIN_STATE_FILE);

  Device* transmitter = nullptr;
  Device* receiver = nullptr;
  std::vector<HAL_Pin*> logicAnalyzerPins;

  switch (protocol){
  case Protocol::I2C:
    I2C_HAL_init();
    console_init(consolePrint);

    transmitter = new I2CDevice("I2C_Transmitter", TRANSMITTER_ADDRESS, I2C_Role::MASTER);
    receiver = new I2CDevice("I2C_Receiver", RECEIVER_ADDRESS, I2C_Role::SLAVE);

    logicAnalyzerPins.insert(logicAnalyzerPins.end(), {
      HAL_SclPin(), 
      HAL_SdaPin(), 
      ((I2CDevice*) transmitter)->getConfig().sdaOutPin, 
      ((I2CDevice*) transmitter)->getConfig().sclOutPin, 
      ((I2CDevice*) receiver)->getConfig().sdaOutPin,
      ((I2CDevice*) receiver)->getConfig().sclOutPin
    });

    logicAnalyzerPins.push_back(HAL_SclPin());
    logicAnalyzerPins.push_back(HAL_SdaPin());
    break;

  case Protocol::SPI:
    SPI_HAL_init();

    transmitter = new SPIDevice("SPI_Transmitter", SPI_Role::SPI_MASTER);
    receiver = new SPIDevice("SPI_Receiver", SPI_Role::SPI_SLAVE);
    break;

  default:
    break;
  }

  std::thread logicAnalyzerProbeThread([&transmitter, &receiver, &logicAnalyzerPins](){
    std::string filename = PIN_STATE_FILE;
    while(true){
      if(transmitter->isTransmissionRunning()){
        std::ofstream ofs(filename, std::ios::app);
        if(ofs.is_open()){
          for(HAL_Pin* pin : logicAnalyzerPins){
            ofs << HAL_pinRead(pin) << " ";
          }
          ofs << std::endl;

          ofs.close();
        } else {
          std::cout << "Unable to open file for writing log\n";
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
      }
    }
  });

  std::thread userInputThread([&](){
    userInputThreadFunction(*transmitter, *receiver);
  });


  if (logicAnalyzerProbeThread.joinable()){
    logicAnalyzerProbeThread.join();
  }

  if (userInputThread.joinable()) {
    userInputThread.join();
  }

  delete transmitter;
  delete receiver;

  return 0;
}