#include <iostream>
#include <fstream>
#include <string>
#include <vector>


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

int main(int argc, char** argv){
  std::cout << "DesktopDemo Start.\n";

  I2C_Role role = I2C_Role::MASTER;

  // Process command-line arguments
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg.substr(0, 7) == "--role=") {
      char roleValue = arg[7];
      if (roleValue == '1') {
        role = I2C_Role::MASTER;
      } else if (roleValue == '2') {
        role = I2C_Role::SLAVE;
      } else {
        std::cerr << "Invalid role value. Use --role=1 for MASTER or --role=2 for SLAVE." << std::endl;
        return 1; // exit with error
      }
    }
  }

  HAL_Pin sclOutPin, sdaOutPin, sclInPin, sdaInPin;
  std::string roleString = role == I2C_Role::MASTER ? "MASTER" : "SLAVE";

  HAL_registerPin(&sclOutPin, (roleString + "_SCL_OUT").c_str());
  HAL_registerPin(&sdaOutPin, (roleString + "_SDA_OUT").c_str());

  HAL_registerPin(&sclInPin, (roleString + "_SCL_IN").c_str());
  HAL_registerPin(&sdaInPin, (roleString + "_SDA_IN").c_str());

  I2C_Config i2c_config {
    .addr = role == I2C_Role::MASTER ? (uint8_t) 51 : (uint8_t) 52,
    .loggingLevel = 4,
    .role = role,
    .sclOutPin = HAL_pinSetup(&sclOutPin, nullptr, 1, HAL_PullupConfig::PULLUP_ENABLE),
    .sdaOutPin = HAL_pinSetup(&sdaOutPin, nullptr, 2, HAL_PullupConfig::PULLUP_ENABLE),
    .sclInPin = HAL_pinSetup(&sclInPin, nullptr, 3, HAL_PullupConfig::PULLUP_ENABLE),
    .sdaInPin = HAL_pinSetup(&sdaInPin, nullptr, 4, HAL_PullupConfig::PULLUP_ENABLE),
    .print_str = &print_str,
    .print_num = &print_num,
  };

  I2C_init(&i2c_config);

  while(true){
    // read user input from stdin and service commands for writing to the pins and reading from the pins
    std::cout << "> ";
    std::string input;
    std::getline(std::cin, input);

    if(input == "exit"){
      break;
    } else if(input == "read scl"){
      std::cout << (HAL_pinRead(&sclInPin) == HIGH ? "HIGH" : "LOW") << "\n";
    } else if(input == "read sda"){
      std::cout << (HAL_pinRead(&sdaInPin) == HIGH ? "HIGH" : "LOW") << "\n";
    } else if(input == "write scl HIGH"){
      HAL_pinWrite(&sclOutPin, HIGH);
    } else if(input == "write scl LOW"){
      HAL_pinWrite(&sclOutPin, LOW);
    } else if(input == "write sda HIGH"){
      HAL_pinWrite(&sdaOutPin, HIGH);
    } else if(input == "write sda LOW"){
      HAL_pinWrite(&sdaOutPin, LOW);
    } else {
      std::cout << "Invalid input\n";
      std::cout << "Valid inputs are:\n";
      std::cout << "exit\n";
      std::cout << "read scl\n";
      std::cout << "read sda\n";
      std::cout << "write scl HIGH\n";
      std::cout << "write scl LOW\n";
      std::cout << "write sda HIGH\n";
      std::cout << "write sda LOW\n";
    }
  }
  

  return 0;
}