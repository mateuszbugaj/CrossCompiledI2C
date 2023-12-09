#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <queue>
#include <string.h>
#include <fstream>

extern "C" {
  #include <I2C_HAL.h>
  #include <I2C_HAL_DESKTOP.h>
  #include <I2C.h>
  #include <console.h>
}

#define PIN_STATE_FILE "output/pins.log"
#define TRANSMITTER_LOG_FILE "output/master.log"
#define RECEIVER_LOG_FILE "output/slave.log"

#define TRANSMITTER_ADDRESS 51
#define RECEIVER_ADDRESS 52

volatile bool logicAnalyzerProbeRunning = false;

class Device {
  private:
    std::string name;
    I2C_Config I2C_config;
    std::thread executionTread;
    std::queue<std::string> instructions;
    bool transmissionRunning = false;

  public:
    Device(std::string name, uint8_t addreess, I2C_Role role){
      this->name = name;

      I2C_config = {
        .addr = addreess,
        .loggingLevel = 4,
        .role = role,
        .sclOutPin = HAL_pinSetup(SCL_OUT),
        .sdaOutPin = HAL_pinSetup(SDA_OUT),
        .sclInPin = HAL_pinSetup(SCL_IN),
        .sdaInPin = HAL_pinSetup(SDA_IN),
        .timeUnit = 150,
        .print_str = Device::printStr,
        .print_num = Device::printNum
      };

      I2C_init(&I2C_config);

      executionTread = std::thread([this](){
        this->executionThreadFunction();
      });
    }

    void executionThreadFunction(){
      while(true){
        if(!instructions.empty()){
          std::string instruction = instructions.front();
          instructions.pop();

          transmissionRunning = true;
          console_parse(&I2C_config, instruction.c_str());
          transmissionRunning = false;
        }

        if(I2C_config.role == I2C_Role::SLAVE){
          I2C_read(&I2C_config);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }

    ~Device(){
      if (executionTread.joinable()) {
        executionTread.join();
      }

      std::cout << "Deleting " << name << " device" << std::endl;
      delete I2C_config.sclInPin;
      delete I2C_config.sclOutPin;
      delete I2C_config.sdaInPin;
      delete I2C_config.sdaOutPin;
    }

    static void printToFile(std::string str, I2C_Role role){
      std::string filename = role == I2C_Role::MASTER ? TRANSMITTER_LOG_FILE : RECEIVER_LOG_FILE;
      char strCopy[50] {0};

      // Strip trailing newlines
      for(int i = 0; i < str.length(); i++){
        if(str[i] != '\r'){
          strCopy[i] = str[i];
        } else {
          strCopy[i] = '\0';
          break;
        }
      }

      std::ofstream ofs(filename, std::ios::app);
      if(ofs.is_open()){
        ofs << strCopy;
        ofs.close();
      } else {
        std::cout << "Unable to open file for writing log\n";
      }
    }

    static void printStr(I2C_Role role, char str[]){
      printToFile(str, role);

      std::string colorCode = role == I2C_Role::MASTER ? "\033[31m" : "\033[32m";
      std::cout << colorCode << str << "\033[0m";
    }

    static void printNum(I2C_Role role, uint16_t num){
      printToFile(std::to_string(num), role);

      std::string colorCode = role == I2C_Role::MASTER ? "\033[31m" : "\033[32m";
      std::cout << colorCode << int(num) << "\033[0m";
    }

    std::string getName(){
      return name;
    }

    I2C_Config getI2CConfig(){
      return I2C_config;
    }

    void send(std::string instruction){
      instructions.push(instruction);
    }

    bool isTransmissionRunning(){
      return transmissionRunning;
    }
};

void consolePrint(char str[]){
  std::cout << str;
}

void clearFile(std::string file){
  std::ofstream ofs(file, std::ios::trunc);
  ofs.close();
}

int main(int argc, char** argv){
  clearFile(TRANSMITTER_LOG_FILE);
  clearFile(RECEIVER_LOG_FILE);
  clearFile(PIN_STATE_FILE);
  console_init(consolePrint);

  Device transmitterDevice("Transmitter", TRANSMITTER_ADDRESS, I2C_Role::MASTER);
  Device receiverDevice("Receiver", RECEIVER_ADDRESS, I2C_Role::SLAVE);

  std::thread logicAnalyzerProbeThread([&transmitterDevice, &receiverDevice](){
    std::string filename = PIN_STATE_FILE;
    while(true){
      if(transmitterDevice.isTransmissionRunning()){
        std::ofstream ofs(filename, std::ios::app);
        if(ofs.is_open()){
          ofs 
            << HAL_pinRead(HAL_SclPin()) << " " 
            << HAL_pinRead(HAL_SdaPin()) << " "
            << HAL_pinRead(transmitterDevice.getI2CConfig().sdaOutPin) << " "
            << HAL_pinRead(transmitterDevice.getI2CConfig().sclOutPin) << " "
            << HAL_pinRead(receiverDevice.getI2CConfig().sdaOutPin) << " "
            << HAL_pinRead(receiverDevice.getI2CConfig().sclOutPin) << std::endl;

          ofs.close();
        } else {
          std::cout << "Unable to open file for writing log\n";
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
      }
    }
  });

  std::thread userInputThread([&transmitterDevice, &receiverDevice](){
    Device* device = &transmitterDevice;

    printf("%s (%d):", device->getName().c_str(), device->getI2CConfig().addr);
    while(true){
      std::string input;
      std::getline(std::cin, input);

      if(input == "t") device = &transmitterDevice;
      else if(input == "r") device = &receiverDevice;
      else if(!input.empty()) device->send(input);

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      printf("%s (%d):", device->getName().c_str(), device->getI2CConfig().addr);
    }
  });

  if (logicAnalyzerProbeThread.joinable()){
    logicAnalyzerProbeThread.join();
  }

  if (userInputThread.joinable()) {
    userInputThread.join();
  }

  return 0;
}