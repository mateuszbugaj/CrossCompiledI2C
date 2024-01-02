#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <queue>
#include <string.h>
#include <fstream>

#include <LogicAnalyzerProbe.hpp>
#include <Device.hpp>
#include <I2CDevice.hpp>
#include <SPIDevice.hpp>

extern "C" {
  #include <I2C_HAL.h>
  #include <I2C_HAL_DESKTOP.h>
  #include <I2C.h>
  #include <SPI_HAL.h>
  #include <SPI.h>
  #include <console.h>
}
#include <SPI_HAL_DESKTOP.hpp>

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
  SPI_HAL_PinManager* SPI_HAL_pinManager = nullptr;

  switch (protocol){
  case Protocol::I2C:
    I2C_HAL_init();
    console_init(consolePrint);

    transmitter = new I2CDevice("I2C_Transmitter", TRANSMITTER_ADDRESS, I2C_Role::MASTER);
    receiver = new I2CDevice("I2C_Receiver", RECEIVER_ADDRESS, I2C_Role::SLAVE);

    static LogicAnalyzerProbe<HAL_Pin, HAL_PinLevel> logicAnalyzerProbeI2C({transmitter, receiver}, {
      HAL_SclPin(), 
      HAL_SdaPin(), 
      ((I2CDevice*) transmitter)->getConfig().sdaOutPin, 
      ((I2CDevice*) transmitter)->getConfig().sclOutPin, 
      ((I2CDevice*) receiver)->getConfig().sdaOutPin,
      ((I2CDevice*) receiver)->getConfig().sclOutPin
    }, HAL_pinRead);

    break;

  case Protocol::SPI:
    SPI_HAL_pinManager = new SPI_HAL_PinManager();
    SPI_HAL_setPinManager(SPI_HAL_pinManager);

    common_SPI_consoleInit(consolePrint);
    transmitter = new SPIDevice("SPI_Transmitter", SPI_Role::SPI_MASTER, SPI_HAL_pinManager);
    receiver = new SPIDevice("SPI_Receiver", SPI_Role::SPI_SLAVE, SPI_HAL_pinManager);

    static LogicAnalyzerProbe<SPI_HAL_Pin, SPI_HAL_PinLevel> logicAnalyzerProbeSPI({transmitter, receiver}, {
      SPI_HAL_pinManager->mosiPin, 
      SPI_HAL_pinManager->misoPin, 
      SPI_HAL_pinManager->sckPin
    }, SPI_HAL_pinRead);

    ((SPIDevice*) transmitter)->addSlaveDevice((SPIDevice*) receiver);
    break;

  default:
    break;
  }

  std::thread userInputThread([&](){
    userInputThreadFunction(*transmitter, *receiver);
  });

  if (userInputThread.joinable()) {
    userInputThread.join();
  }

  delete transmitter;
  delete receiver;

  return 0;
}