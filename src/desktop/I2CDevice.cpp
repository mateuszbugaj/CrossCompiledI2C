#include <thread>
#include <queue>
#include <iostream>
#include <fstream>

#include <I2CDevice.hpp>

static void printToFile(std::string str, I2C_Role role);
static void printStr(I2C_Role role, char str[]);
static void printNum(I2C_Role role, uint16_t num);

I2CDevice::I2CDevice(std::string name, uint8_t addreess, I2C_Role role){
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
    .print_str = printStr,
    .print_num = printNum
  };

  I2C_init(&I2C_config);

  executionTread = std::thread([this](){
    this->executionThreadFunction();
  });
}

void I2CDevice::executionThreadFunction(){
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

I2CDevice::~I2CDevice(){
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

I2C_Config I2CDevice::getConfig(){
  return I2C_config;
}