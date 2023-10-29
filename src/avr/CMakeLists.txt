add_avr_executable(AvrDemo AvrDemo.c)
add_avr_library(USART USART.c USART.h)
avr_target_link_libraries(AvrDemo GenericI2C USART)
avr_target_compile_definitions(GenericI2C PRIVATE AVR=1)