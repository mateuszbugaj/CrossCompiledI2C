#include <bitset>

#include <SPIDevice.hpp>

// static void printToFile(std::string str, SPI_Role role);
static void printStr(SPI_Role role, char str[]);
static void printNum(SPI_Role role, uint16_t num);

SPIDevice::SPIDevice(std::string name, SPI_Role role, SPI_HAL_PinManager* pinManager){
  this->name = name;

  SPI_HAL_Pin* MISO;
  SPI_HAL_Pin* MOSI;
  SPI_HAL_Pin* SCK;
  SPI_HAL_Pin* SS;

  if(role == SPI_Role::SPI_MASTER){
    MISO = pinManager->createPin(SPI_HAL_PinRole::MISO_IN);
    MOSI = pinManager->createPin(SPI_HAL_PinRole::MOSI_OUT);
    SCK = pinManager->createPin(SPI_HAL_PinRole::SCK_OUT);
  } else {
    MISO = pinManager->createPin(SPI_HAL_PinRole::MISO_OUT);
    MOSI = pinManager->createPin(SPI_HAL_PinRole::MOSI_IN);
    SCK = pinManager->createPin(SPI_HAL_PinRole::SCK_IN);
  }

  SS = pinManager->createPin(SPI_HAL_PinRole::SS);

  SPI_config = {
    .role = role,
    .loggingLevel = SPI_LoggingLevel::SPI_BITS,
    .MOSI = MOSI,
    .MISO = MISO,
    .SS = SS,
    .SCK = SCK,
    .print_str = printStr,
    .print_num = printNum
  };

  SPI_init(&SPI_config);

  executionTread = std::thread([this](){
    this->executionThreadFunction();
  });
}

void SPIDevice::executionThreadFunction(){
  while(isRunning){
    if(SPI_config.role == SPI_SLAVE){
      SPI_read(&SPI_config);
    }

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
  isRunning = false;
  if(executionTread.joinable()){
    executionTread.join();
  }
  
}

static void printToFile(std::string str, SPI_Role role){
  std::string filename = role == SPI_Role::SPI_MASTER ? TRANSMITTER_LOG_FILE : RECEIVER_LOG_FILE;
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

static void printStr(SPI_Role role, char str[]){
  printToFile(str, role);

  std::string colorCode = role == SPI_Role::SPI_MASTER ? "\033[31m" : "\033[32m";
  std::cout << colorCode << str << "\033[0m";
}

static void printNum(SPI_Role role, uint16_t num){
  printToFile(std::bitset<8>(num).to_string(), role);

  std::string colorCode = role == SPI_Role::SPI_MASTER ? "\033[31m" : "\033[32m";
  std::cout << colorCode << int(num) << " (" << std::bitset<8>(num) << ")" << "\033[0m";
}

SPI_Config* SPIDevice::getConfig(){
  return &SPI_config;
}

void SPIDevice::addSlaveDevice(SPIDevice* slave){
  std::string slaveName = slave->getName();

  std::cout << slaveName << " added as slave to " << this->getName() << '\n';
  slaveDevices[slaveName] = slave;
}

void SPIDevice::sendByte(uint8_t byte, std::string receiverName){
  SPIDevice* receiver = slaveDevices[receiverName];

  if(receiver == nullptr){
    std::cout << receiverName << " not found\n";
  } else {
    SPI_send(&SPI_config, byte, receiver->getConfig()->SS);
  }
}