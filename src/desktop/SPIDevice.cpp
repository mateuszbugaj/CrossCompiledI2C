#include <SPIDevice.hpp>

// static void printToFile(std::string str, SPI_Role role);
static void printStr(SPI_Role role, char str[]);
static void printNum(SPI_Role role, uint16_t num);

SPIDevice::SPIDevice(std::string name, SPI_Role role){
  this->name = name;

  SPI_HAL_Pin* MISO;
  SPI_HAL_Pin* MOSI;
  SPI_HAL_Pin* SCK;
  SPI_HAL_Pin* SS;

  if(role == SPI_Role::SPI_MASTER){
    MISO = SPI_HAL_pinSetup(SPI_HAL_PinRole::MISO_IN);
    MOSI = SPI_HAL_pinSetup(SPI_HAL_PinRole::MOSI_OUT);
    SCK = SPI_HAL_pinSetup(SPI_HAL_PinRole::SCK_OUT);
    SS = SPI_HAL_pinSetup(SPI_HAL_PinRole::SS_OUT);
  } else {
    MISO = SPI_HAL_pinSetup(SPI_HAL_PinRole::MISO_OUT);
    MOSI = SPI_HAL_pinSetup(SPI_HAL_PinRole::MOSI_IN);
    SCK = SPI_HAL_pinSetup(SPI_HAL_PinRole::SCK_IN);
    SS = SPI_HAL_pinSetup(SPI_HAL_PinRole::SS_IN);
  }

  SPI_Config SPI_config = {
    .role = role,
    .loggingLevel = SPI_LoggingLevel::SPI_BITS,
    .MOSI = MOSI,
    .MISO = MISO,
    SCK = SCK,
    SS = SS,
    .print_str = printStr,
    .print_num = printNum
  };

  SPI_init(&SPI_config);

  executionTread = std::thread([this](){
    this->executionThreadFunction();
  });
}

void SPIDevice::executionThreadFunction(){
  while(true){
    if(!instructions.empty()){
      transmissionRunning = true;
      std::string instruction = instructions.front();
      instructions.pop();

      std::cout << "Consumed: " << instruction << std::endl;
      
      transmissionRunning = false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

SPIDevice::~SPIDevice(){
  std::cout << "Deleting " << name << " device" << std::endl;
}

static void printStr(SPI_Role role, char str[]){
  // printToFile(str, role);

  std::string colorCode = role == SPI_Role::SPI_MASTER ? "\033[31m" : "\033[32m";
  std::cout << colorCode << str << "\033[0m";
}

static void printNum(SPI_Role role, uint16_t num){
  // printToFile(std::to_string(num), role);

  std::string colorCode = role == SPI_Role::SPI_MASTER ? "\033[31m" : "\033[32m";
  std::cout << colorCode << int(num) << "\033[0m";
}

SPI_Config* SPIDevice::getConfig(){
  return &SPI_config;
}