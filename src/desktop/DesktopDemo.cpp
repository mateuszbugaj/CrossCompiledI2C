#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <signal_logger.hpp>
#include <I2C_HAL.h>
extern "C" {
  #include <I2C.h>
}

void print_str(char str[]){
  std::cout << str;
}

void print_num(uint8_t num){
  std::cout << int(num);
}

std::vector<HAL_PinLevel> tokenize(std::string s, std::string delimiter){
  size_t pos = 0;
  std::string token;
  std::vector<HAL_PinLevel> result;

  while ((pos = s.find(delimiter)) != std::string::npos) {
      token = s.substr(0, pos);
      result.push_back(token == "0" ? HAL_PinLevel::LOW : HAL_PinLevel::HIGH);
      s.erase(0, pos + delimiter.length());
  }

  result.push_back(s == "0" ? HAL_PinLevel::LOW : HAL_PinLevel::HIGH);

  return result;
}

int main(){
  std::cout << "DesktopDemo Start.\n";
  HAL_setLoggingCallback(SL_logPinState);

  bool roleSelected = false;
  I2C_Role role = I2C_Role::MASTER;
  while(!roleSelected){
    std::cout << "Select role:\n1) Master\n2) Slave\n> ";
    int roleSelection;
    std::cin >> roleSelection;
    roleSelected = true;
    if(roleSelection == 1){
      role = I2C_Role::MASTER;
    } else if(roleSelection == 2){
      role = I2C_Role::SLAVE;
    } else {
      std::cout << "404 Role Not Found.\n";
      roleSelected = false;
    }
  }

  HAL_Pin sclOutPin, sdaOutPin, sclInPin, sdaInPin;

  I2C_Config i2c_config {
    .addr = role == I2C_Role::MASTER ? (uint8_t) 51 : (uint8_t) 52,
    .loggingLevel = 4,
    .role = role,
    .sclOutPin = pinSetup(&sclOutPin, nullptr, 1, HAL_PullupConfig::PULLUP_ENABLE),
    .sdaOutPin = pinSetup(&sdaOutPin, nullptr, 2, HAL_PullupConfig::PULLUP_ENABLE),
    .sclInPin = pinSetup(&sclInPin, nullptr, 3, HAL_PullupConfig::PULLUP_ENABLE),
    .sdaInPin = pinSetup(&sdaInPin, nullptr, 4, HAL_PullupConfig::PULLUP_ENABLE),
    .print_str = &print_str,
    .print_num = &print_num,
  };

  I2C_init(&i2c_config);

  HAL_pinWrite(&sclOutPin, HAL_PinLevel::HIGH);

  // std::cout << "Reading the input file.\n";
  // std::ifstream file("src/desktop/testInput.txt");
  // if(!file.is_open()){
  //   std::cerr << "Failed to open the file.\n";
  //   return 1;
  // }

  // std::string line;
  // std::cout << "SCL|SDA\n";
  // while(std::getline(file, line)){
  //   std::vector<HAL_PinLevel> signals = tokenize(line, " ");

  //   for(int i = 0; i < signals.size(); i++){
  //     std::cout << (signals[i] == HAL_PinLevel::LOW ? " 0 " : " 1 ");
      
  //     if(i != signals.size() - 1){
  //       std::cout << "|";
  //     }
  //   }
  //   std::cout << "\n";

  // }

  // std::cout << "Finished reading the input file.\n";



  return 0;
}