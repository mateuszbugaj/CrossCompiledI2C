#include <stdint.h>

#include <I2C/I2C.h>

I2C_config* cfg;

/*
  Pointer to printing function. Used to print logs for example via USART interface.
  By default, an empty function.
*/
void defaultPrint(char chars[]){}
void defaultPrintNum(uint8_t value){}
void (*printFuncPtr)(char content[]) = &defaultPrint;
void (*printNumFuncPtr)(uint8_t value) = &defaultPrintNum;

void I2C_log(char* content, uint8_t level) {
  if (cfg->loggingLevel >= level) {
    (*printFuncPtr)(content);
    (*printFuncPtr)("\n\r");
  }
}

void I2C_logNum(char name[], uint8_t value, uint8_t level) {
  if (cfg->loggingLevel >= level) {
    (*printFuncPtr)(name);
    (*printFuncPtr)(": ");
    (*printNumFuncPtr)(value);
    (*printFuncPtr)("\n\r");
  }
}

void I2C_setup(I2C_config* config){
  cfg = config;

  if(cfg->print_str != ((void *)0)){
    printFuncPtr = cfg->print_str;
    printNumFuncPtr = cfg->print_num;
  } else {
    cfg->loggingLevel = 0;
  }

  I2C_log("I2C Init", 1);
  I2C_logNum("Addr", 15, 2);

  HAL_pinWrite(cfg->sclPin, HIGH);

  return;
}