cmake_minimum_required(VERSION 3.20)

project(
  CrossCompiledI2C
  DESCRIPTION "I2C library demo for AVR, STM32 and desktop platform."
  LANGUAGES C CXX)

message("C compiler: " ${CMAKE_C_COMPILER} ", C++ compiler: " ${CMAKE_CXX_COMPILER})

# Ugly hack but it is what it is for STM32
if(TARGET_PLATFORM STREQUAL "STM32")
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/stm32/blue-pill.cmake)
endif()

add_subdirectory(lib/GenericI2C)
add_subdirectory(src/common)

if(TARGET_PLATFORM STREQUAL "DESKTOP" OR NOT DEFINED TARGET_PLATFORM)
  set(TARGET_PLATFORM "DESKTOP")
  message("Building for desktop")
  add_subdirectory(src/desktop)
  add_subdirectory(test)
elseif(TARGET_PLATFORM STREQUAL "AVR")
  message("Building for AVR")
  add_subdirectory(src/avr)
elseif(TARGET_PLATFORM STREQUAL "STM32")
  message("Building for STM32")
  add_subdirectory(src/stm32)
else()
  message(FATAL_ERROR "Invalid TARGET_PLATFORM: ${TARGET_PLATFORM}")
endif()