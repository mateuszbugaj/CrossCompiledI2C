add_executable(DesktopDemo DesktopDemo.cpp Device.cpp I2CDevice.cpp SPIDevice.cpp)
target_include_directories(DesktopDemo PUBLIC .)
target_link_libraries(DesktopDemo GenericI2C GenericSPI Console)