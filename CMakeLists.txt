cmake_minimum_required(VERSION 3.20)

project(
  CrossCompiledI2C
  DESCRIPTION "I2C library demo for AVR, STM32 and desktop platform."
  LANGUAGES C CXX)

message("C compiler: " ${CMAKE_C_COMPILER} ", C++ compiler: " ${CMAKE_CXX_COMPILER})

if(TARGET_PLATFORM STREQUAL "DESKTOP")
  message("Building for desktop")
  add_subdirectory(src/desktop)
elseif(TARGET_PLATFORM STREQUAL "AVR")
  message("Building for AVR")
  add_subdirectory(src/avr)
elseif(TARGET_PLATFORM STREQUAL "STM32")
  message("Building for STM32")
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/stm32/blue-pill.cmake)
  add_subdirectory(src/stm32)
else()
  message(FATAL_ERROR "Invalid TARGET_PLATFORM: ${TARGET_PLATFORM}")
endif()

add_subdirectory(lib/GenericI2C)