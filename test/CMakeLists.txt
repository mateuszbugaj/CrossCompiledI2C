add_subdirectory(googletest)
enable_testing()

add_executable(Tests 
  console_test.cpp 
  I2C_HAL_Desktop_Impl_test.cpp 
  SPI_test.cpp 
  ${CMAKE_SOURCE_DIR}/src/desktop/SPIDevice.cpp
  ${CMAKE_SOURCE_DIR}/src/desktop/Device.cpp
  ${CMAKE_SOURCE_DIR}/lib/GenericSPI/src/SPI_HAL_Desktop_Impl.cpp)

target_include_directories(Tests PUBLIC .)
target_include_directories(Tests PUBLIC ${CMAKE_SOURCE_DIR}/src/desktop/)
target_link_libraries(Tests gtest_main Console GenericI2C GenericSPI)

target_compile_options(Tests PRIVATE -g)