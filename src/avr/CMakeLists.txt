add_definitions("-DF_CPU=8000000UL")
add_definitions("-Wall")
add_definitions("-std=c99")
add_definitions("-g")
add_definitions("-D__AVR_ATmega168__")
add_definitions("-D__OPTIMIZE__")
add_definitions("-Os")

# set(AVR 1)

add_avr_executable(AvrDemo AvrDemo.c)
avr_target_include_directories(AvrDemo PUBLIC ${CMAKE_SOURCE_DIR}/include)
avr_target_link_libraries(AvrDemo I2C)
avr_target_link_libraries(AvrDemo HAL)
avr_target_compile_definitions(I2C PRIVATE AVR=1) # Replaced by set(AVR 1)? 
avr_target_compile_definitions(HAL PRIVATE AVR=1)
# target_compile_definitions(I2C PRIVATE DESKTOP=1)

