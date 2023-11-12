#pragma once
#include <I2C.h>

void console_init(I2C_Config* i2c_config, void (*print)(char*));
void console_parse(const char* instruction);