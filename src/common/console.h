#pragma once
#include <I2C.h>

void console_init(void (*print)(char*));
void console_parse(I2C_Config* cfg, const char* instruction);