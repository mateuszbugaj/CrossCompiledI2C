#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>

extern "C" {
  #include <I2C_HAL.h>
  #include <I2C_HAL_DESKTOP.h>
  #include <I2C.h>
  #include <console.h>
}

#define PIN_STATE_FILE "output/pin_states.json"
#define LOGIC_ANALYZER_FILE "output/logic_analyzer_snapshot.txt"

I2C_Role role = I2C_Role::MASTER;
bool logicAnalyzerProbeRunning = false;

std::mutex inputMutex;
std::condition_variable inputCond;
std::string sharedInput;
bool inputAvailable = false;

void consoleInputThread() {
  while (true) {
    std::string input;
    std::getline(std::cin, input);

    {
      std::lock_guard<std::mutex> lock(inputMutex);
      sharedInput = input;
      inputAvailable = true;
    }

    inputCond.notify_one();
  }
}

void print_str(char str[]){
  std::string filename = std::string("output/") + (role == I2C_Role::MASTER ? "master" : "slave") + "_log.txt";

  char strCopy[50] {0};

  // Strip trailing newlines
  for(int i = 0; i < strlen(str); i++){
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

  std::cout << strCopy;
}

void print_num(uint16_t num){
  std::string filename = std::string("output/") + (role == I2C_Role::MASTER ? "master" : "slave") + "_log.txt";
  std::ofstream ofs(filename, std::ios::app);
  if(ofs.is_open()){
    ofs << int(num);
    ofs.close();
  } else {
    std::cout << "Unable to open file for writing log\n";
  }

  std::cout << int(num);
}

void logicAnalyzerSnapshot(){
  // Parse last line of the PIN_STATE_FILE to get the current pin states and save them to a LOGIC_ANALYZER_FILE for the logic analyzer.
  // Each line of the PIN_STATE_FILE is a JSON with the following format:
  // {"timestamp":"...","signals":[{"SLAVE_SCL_OUT":0},{"SLAVE_SDA_OUT":0},{"MASTER_SCL_OUT":0},{"MASTER_SDA_OUT":0}],"SCL":0,"SDA":1}
  // Each line of the LOGIC_ANALYZER_FILE is a binary string with bits separated by spaces which are the output pins and the bus state (SCL and SDA)
  // Example: 0 1 0 1 0 1 0 1

  std::ifstream ifs(PIN_STATE_FILE);
  std::string lastLine;
  if (ifs.is_open()) {
    std::string line;
    while (std::getline(ifs, line)) {
      lastLine = line;
    }
    ifs.close();
  }

  if(lastLine.size() > 0){
    std::string timestamp;
    std::map<std::string, int> pinStates;
    int scl, sda;

    rapidjson::Document d;
    d.Parse(lastLine.c_str());
    timestamp = d["timestamp"].GetString();
    for(auto& signal : d["signals"].GetArray()){
      pinStates[signal.GetObject().MemberBegin()->name.GetString()] = signal.GetObject().MemberBegin()->value.GetInt();
    }

    if(d.HasMember("SCL")){
      scl = d["SCL"].GetInt();
    } else {
      scl = 0;
    }

    if(d.HasMember("SDA")){
      sda = d["SDA"].GetInt();
    } else {
      sda = 0;
    }

    std::ofstream ofs(LOGIC_ANALYZER_FILE, std::ios::app);
    if(ofs.is_open()){
      ofs << sda << " " << scl << " " << pinStates["MASTER_SDA_OUT"] << " " << pinStates["MASTER_SCL_OUT"] << " " << pinStates["SLAVE_SDA_OUT"] << " " << pinStates["SLAVE_SCL_OUT"] << std::endl;
      ofs.close();
    } else {
      std::cout << "Unable to open file for writing logic analyzer snapshot\n";
    }
  }
}

void startLogicAnalyzerProbe(std::chrono::milliseconds interval){  
  while(logicAnalyzerProbeRunning){
    logicAnalyzerSnapshot();
    std::this_thread::sleep_for(interval);
  }
}

void clearFile(std::string file){
  std::ofstream ofs(file, std::ios::trunc);
  ofs.close();
}

int main(int argc, char** argv){
  std::cout << "DesktopDemo Start.\n";

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

  // Clear the log files
  std::string filename = std::string("output/") + (role == I2C_Role::MASTER ? "master" : "slave") + "_log.txt";
  clearFile(filename);

  if(role == I2C_Role::MASTER){
    clearFile(PIN_STATE_FILE);
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
    .sclOutPin = HAL_pinSetup(&sclOutPin, 1),
    .sdaOutPin = HAL_pinSetup(&sdaOutPin, 2),
    .sclInPin = HAL_pinSetup(&sclInPin, 3),
    .sdaInPin = HAL_pinSetup(&sdaInPin, 4),
    .timeUnit = 100,
    .print_str = &print_str,
    .print_num = &print_num,
  };

  I2C_init(&i2c_config);
  console_init(&i2c_config, &print_str);

  // Start console input thread
  std::thread inputThread(consoleInputThread);

  std::cout << "> ";
  while (true) {
    if (role == I2C_Role::SLAVE) {
      I2C_read();
    }

    {
      std::unique_lock<std::mutex> lock(inputMutex);
      if (inputCond.wait_for(lock, std::chrono::milliseconds(10), [] { return inputAvailable; })) {
        std::cout << "> " << sharedInput << std::endl;
        if (sharedInput.find("write") != std::string::npos) {
          logicAnalyzerProbeRunning = true;
          std::thread t(startLogicAnalyzerProbe, std::chrono::milliseconds(10));
          t.detach();
        }
        console_parse(sharedInput.c_str());
        logicAnalyzerProbeRunning = false;
        inputAvailable = false;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}