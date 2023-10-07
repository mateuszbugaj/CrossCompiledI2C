# This file allows to choose the appropriate toolchain for the selected platform.

set(TARGET_PLATFORM "DESKTOP" CACHE STRING "Target platform for the build")

if(TARGET_PLATFORM STREQUAL "DESKTOP")
  find_program(CC gcc REQUIRED)
  find_program(CXX g++ REQUIRED)
elseif(TARGET_PLATFORM STREQUAL "AVR")
  set(AVR_UPLOADTOOL avrdude)
  set(AVR_PROGRAMMER USBasp)
  set(AVR_UPLOADTOOL_PORT usb)
  set(AVR_MCU atmega168)
  include("${CMAKE_SOURCE_DIR}/cmake/generic-gcc-avr.cmake")
  find_program(CC avr-gcc REQUIRED)
  find_program(CXX avr-g++ REQUIRED)

  add_definitions("-DF_CPU=8000000UL")
  add_definitions("-Wall")
  add_definitions("-std=c99")
  add_definitions("-g")
  add_definitions("-D__AVR_ATmega168__")
  add_definitions("-D__OPTIMIZE__")
  add_definitions("-Os")  
endif()

set(CMAKE_C_COMPILER ${CC} CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER ${CXX} CACHE STRING "C++ compiler" FORCE)