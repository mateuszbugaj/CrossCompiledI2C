#pragma once
#include <SPI.h>

void common_SPI_consoleInit(void (*print)(char*));
void common_SPI_consoleParse(SPI_Config* cfg, const char* instruction);