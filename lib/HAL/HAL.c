#include <HAL/HAL.h>

#ifdef DESKTOP
/* On the desktop, the writing and reading functions will use external files */
// TODO functions should be inlined
#include <stdio.h>
void HAL_setPinDirection(HAL_Pin* pin, HAL_PinDirection direction){
  printf("Selecting pin %d direction: %s\n", pin->pin, direction == INPUT ? "INPUT" : "OUTPUT");
}

void HAL_pinWrite(HAL_Pin* pin, HAL_PinLevel level){
  printf("Writing to pin %d: %d\n", pin->pin, level);
}

HAL_PinLevel HAL_pinRead(HAL_Pin* pin){
  printf("Reading from pin %d\n", pin->pin);
  // TODO
}
#endif

#ifdef AVR
/* On the AVR, the writing and reading functions will use pins directly */
#include <avr/io.h>

void HAL_setPinDirection(HAL_Pin* pin, HAL_PinDirection direction){
  if (direction == OUTPUT) {
    *(pin->port - 1) |= (1 << pin->pin); // DDRx is one address below PORTx
  } else {
    *(pin->port - 1) &= ~(1 << pin->pin);
    if(pin->pullup == PULLUP_ENABLE){
      *(pin->port - 2) |= (1 << pin->pin);
    }
  }
}

void HAL_pinWrite(HAL_Pin* pin, HAL_PinLevel level){
  if (level == HIGH) {
    *(pin->port) |= (1 << pin->pin);
  } else {
    *(pin->port) &= ~(1 << pin->pin);
  }
}

HAL_PinLevel HAL_pinRead(HAL_Pin* pin){
  return (*(pin->port - 2) & (1 << pin->pin)) ? HIGH : LOW; // PINx is two addresses below PORTx
}

#endif

#ifdef STM32
/* On the STM32, the writing and reading functions will use pins via libopencm3 framework */

inline void HAL_setPinDirection(HAL_Pin* pin, HAL_PinDirection direction){
  // TODO
}

inline void HAL_pinWrite(HAL_Pin* pin, HAL_PinLevel level){
  // TODO
}

inline HAL_PinLevel HAL_pinRead(HAL_Pin* pin){
  // TODO
}

#endif