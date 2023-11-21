#include <console.h>
#include <itoa.h>
#include <string.h>

#ifdef DESKTOP
#define NEW_LINE "\n"
#else
#define NEW_LINE "\n\r"
#endif

I2C_Config* i2c_cfg;
void (*console_print)(char*) = 0;
typedef void (*CommandHandler)(const char*);

typedef struct {
  const char* command;
  CommandHandler handler;
} Command;

void handleGetRole(const char* args) {
  console_print(i2c_cfg->role == MASTER ? "MASTER" : "SLAVE");
  console_print(NEW_LINE);
}

void handleSetRole(const char* args) {
  char role[10];
  getArg(args, role, 0);
  if (strcmp(role, "master") == 0) {
    i2c_cfg->role = MASTER;
  } else if (strcmp(role, "slave") == 0) {
    i2c_cfg->role = SLAVE;
  }
}

void handleGetAddress(const char* args) {
  char address[5];
  itoa(i2c_cfg->addr, address, 10);
  console_print(address);
  console_print(NEW_LINE);
}

void handleSetAddr(const char* args) {
  char address[5];
  getArg(args, address, 0);
  int addr = atoi(address);
  if (addr < 0 || addr > 127) {
    return;
  }
  i2c_cfg->addr = addr;
}

void handleGetTimeUnit(const char* args) {
  char timeUnit[5];
  itoa(i2c_cfg->timeUnit, timeUnit, 10);
  console_print(timeUnit);
  console_print(NEW_LINE);
}

void handleSetTimeUnit(const char* args) {
  char timeUnit[5];
  getArg(args, timeUnit, 0);
  int timeUnitInt = atoi(timeUnit);
  if (timeUnitInt < 0 || timeUnitInt > 1000) {
    return;
  }
  i2c_cfg->timeUnit = timeUnitInt;
}

void handleGetLoggingLevel(const char* args) {
  char loggingLevel[5];
  itoa(i2c_cfg->loggingLevel, loggingLevel, 10);
  console_print(loggingLevel);
  console_print(NEW_LINE);
}

void handleSetLoggingLever(const char* args) {
  char loggingLevel[5];
  getArg(args, loggingLevel, 0);
  int loggingLevelInt = atoi(loggingLevel);
  if (loggingLevelInt < 0 || loggingLevelInt > 5) {
    return;
  }
  i2c_cfg->loggingLevel = loggingLevelInt;
}

void handleGetSclLevel(const char* args){
  char sclLevel[5];
  itoa(HAL_pinRead(i2c_cfg->sclInPin), sclLevel, 10);
  console_print(sclLevel);
  console_print(NEW_LINE);
}

void handleGetSdaLevel(const char* args){
  char sdaLevel[5];
  itoa(HAL_pinRead(i2c_cfg->sdaInPin), sdaLevel, 10);
  console_print(sdaLevel);
  console_print(NEW_LINE);
}

Command getCommandTable[] = {
  {"role", handleGetRole},
  {"address", handleGetAddress},
  {"time_unit", handleGetTimeUnit},
  {"logging_level", handleGetLoggingLevel},
  {"scl", handleGetSclLevel},
  {"sda", handleGetSdaLevel}
};

Command setCommandTable[] = {
  {"role", handleSetRole},
  {"address", handleSetAddr},
  {"time_unit", handleSetTimeUnit},
  {"logging_level", handleSetLoggingLever}
};

void handleWriteByte(const char* args) {
  char byte[5];
  getArg(args, byte, 0);
  console_print("B:");
  console_print(byte);
  console_print(NEW_LINE);
  int byteInt = atoi(byte);
  if (byteInt < 0 || byteInt > 255) {
    return;
  }

  I2C_write(byteInt);
}

void handleWriteAddress(const char* args) {
  char address[5];
  char direction[5];
  getArg(args, address, 0);
  getArg(args, direction, 1);

  int addressInt = atoi(address);
  if (addressInt < 0 || addressInt > 127) {
    return;
  }

  if (strcmp(direction, "r") == 0) {
    I2C_writeAddress(addressInt, READ);
  } else if (strcmp(direction, "w") == 0) {
    I2C_writeAddress(addressInt, WRITE);
  }
}

void handleWriteStartCondition(const char* args) {
  I2C_sendStartCondition();
}

void handleWriteStopCondition(const char* args) {
  I2C_sendStopCondition();
}

void handleWriteToCondition(const char* args) {
  char address[5];
  char payload[5];
  getArg(args, address, 0);
  getArg(args, payload, 1);
  int addressInt = atoi(address);
  int payloadInt = atoi(payload);

  if (addressInt < 0 || addressInt > 127) {
    return;
  }

  I2C_sendStartCondition();
  bool ack = I2C_writeAddress(addressInt, WRITE);
  if(ack){
    I2C_write(payloadInt);
  }

  I2C_sendStopCondition();
}

Command writeCommandTable[] = {
  {"byte", handleWriteByte},
  {"addr", handleWriteAddress},
  {"start", handleWriteStartCondition},
  {"stop", handleWriteStopCondition},
  {"to", handleWriteToCondition}
};

void console_init(I2C_Config* i2c_config, void (*print)(char*)){
  console_print = print;
  i2c_cfg = i2c_config;
}

void getArg(const char* args, char* arg, int argNum) {
  int charNum = 0;
  int argCount = 0;
  int argCharNum = 0;

  while(args[charNum] != '\0'){
    if(argCount == argNum && args[charNum] != ' '){
      arg[argCharNum] = args[charNum];
      argCharNum++;
    }

    if(args[charNum] == ' '){
      argCount++;
    }

    charNum++;
  }

  arg[argCharNum] = '\0';
}

void console_parse(const char* instruction) {
  char command[5];
  getArg(instruction, command, 0);

  Command* commandTable;
  int commandTableSize;

  if(strcmp(command, "get") == 0){
    commandTable = getCommandTable;
    commandTableSize = sizeof(getCommandTable) / sizeof(Command);
  } else if(strcmp(command, "set") == 0){
    commandTable = setCommandTable;
    commandTableSize = sizeof(setCommandTable) / sizeof(Command);
  } else if(strcmp(command, "write") == 0){
    commandTable = writeCommandTable;
    commandTableSize = sizeof(writeCommandTable) / sizeof(Command);
  } else {
    return;
  }

  bool found = false;
  for (int i = 0; i < commandTableSize; ++i) {
    char arg1[15];
    getArg(instruction, arg1, 1);
    if (strcmp(commandTable[i].command, arg1) == 0) {
      found = true;

#ifdef DESKTOP
      // TODO: This is a hack
      commandTable[i].handler(instruction + strlen(command) + 2);
#else
      commandTable[i].handler(instruction + strlen(command) + strlen(arg1) + 2);
#endif
    }
  }

  if (!found) {
    console_print("Commands:");
    console_print(NEW_LINE);
    for (int i = 0; i < commandTableSize; ++i) {
      console_print("  ");
      console_print(commandTable[i].command);
      console_print(NEW_LINE);
    }
  }
}