#include <iostream>
#include <fstream>
#include <string>
#include <vector>

extern "C" {
  #include <HAL/HAL.h>
  #include <I2C/I2C.h>
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

extern volatile uint8_t PORT = 0;
int main(){
  std::cout << "DesktopDemo Start.\n";

  HAL_Pin sclPin = { &PORT, 1, PULLUP_ENABLE };

  I2C_config i2c_config {
    .addr = 51,
    .print_str = &print_str,
    .print_num = &print_num,
    .loggingLevel = 4,
    .sclPin = &sclPin
  };

  I2C_setup(&i2c_config);

  std::cout << "Reading the input file.\n";
  std::ifstream file("src/desktop/testInput.txt");
  if(!file.is_open()){
    std::cerr << "Failed to open the file.\n";
    return 1;
  }

  std::string line;
  std::cout << "SCL|SDA\n";
  while(std::getline(file, line)){
    std::vector<HAL_PinLevel> signals = tokenize(line, " ");

    for(int i = 0; i < signals.size(); i++){
      std::cout << (signals[i] == HAL_PinLevel::LOW ? " 0 " : " 1 ");
      
      if(i != signals.size() - 1){
        std::cout << "|";
      }
    }
    std::cout << "\n";

  }

  std::cout << "Finished reading the input file.\n";



  return 0;
}