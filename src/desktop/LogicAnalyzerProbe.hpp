#include <vector>
#include <thread>
#include <iostream>

#include <Device.hpp>
extern "C" {
  #include <I2C_HAL.h>
  #include <I2C_HAL_DESKTOP.h>
}

template <typename T, typename H>
class LogicAnalyzerProbe {
  private:
    std::thread executionTread;

  public:
    std::vector<Device*> devices;
    std::vector<T*> pins;
    H (*pinReadCallback)(T* pin);

    LogicAnalyzerProbe(std::vector<Device*> devices, std::vector<T*> pins, H (*pinReadCallback)(T* pin)){
    this->devices = devices;
    this->pins = pins;
    this->pinReadCallback = pinReadCallback;

    executionTread = std::thread([this](){
        std::string filename = PIN_STATE_FILE;
        while(true){
          bool transmissionRunning = false;
          for(Device* transmitter : this->devices){
            transmissionRunning |= transmitter->isTransmissionRunning();
          }

          if(transmissionRunning){
            std::ofstream ofs(filename, std::ios::app);
            if(ofs.is_open()){
              for(T* pin : this->pins){
                ofs << this->pinReadCallback(pin) << " ";
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
    }

    ~LogicAnalyzerProbe(){
      if(executionTread.joinable()){
        executionTread.join();
      }
    }
};