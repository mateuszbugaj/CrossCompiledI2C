#include <console.h>
#include <itoa.h>
#include <string.h>

#ifdef DESKTOP
#define NEW_LINE "\n"
#else
#define NEW_LINE "\n\r"
#endif

static void (*console_print)(char*) = 0;
typedef void (*CommandHandler)(I2C_Config* cfg, const char*);

typedef struct {
  const char* command;
  CommandHandler handler;
} Command;

static void getArg(const char* args, char* arg, int argNum) {
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

static void handleGetRole(I2C_Config* i2c_cfg,  char* args) {
  console_print(i2c_cfg->role == MASTER ? "MASTER" : "SLAVE");
  console_print(NEW_LINE);
}

static void handleSetRole(I2C_Config* i2c_cfg, const char* args) {
  char role[10];
  getArg(args, role, 0);
  if (strcmp(role, "master") == 0) {
    i2c_cfg->role = MASTER;
  } else if (strcmp(role, "slave") == 0) {
    i2c_cfg->role = SLAVE;
  }
}

void handleGetAddress(I2C_Config* i2c_cfg, const char* args) {
  char address[5];
  itoa(i2c_cfg->addr, address, 10);
  console_print(address);
  console_print(NEW_LINE);
}

void handleSetAddr(I2C_Config* i2c_cfg, const char* args) {
  char address[5];
  getArg(args, address, 0);
  int addr = atoi(address);
  if (addr < 0 || addr > 127) {
    return;
  }
  i2c_cfg->addr = addr;
}

void handleGetTimeUnit(I2C_Config* i2c_cfg, const char* args) {
  char timeUnit[5];
  itoa(i2c_cfg->timeUnit, timeUnit, 10);
  console_print(timeUnit);
  console_print(NEW_LINE);
}

void handleSetTimeUnit(I2C_Config* i2c_cfg, const char* args) {
  char timeUnit[5];
  getArg(args, timeUnit, 0);
  int timeUnitInt = atoi(timeUnit);
  if (timeUnitInt < 0 || timeUnitInt > 1000) {
    return;
  }
  i2c_cfg->timeUnit = timeUnitInt;
}

void handleGetLoggingLevel(I2C_Config* i2c_cfg, const char* args) {
  char loggingLevel[5];
  itoa(i2c_cfg->loggingLevel, loggingLevel, 10);
  console_print(loggingLevel);
  console_print(NEW_LINE);
}

void handleSetLoggingLever(I2C_Config* i2c_cfg, const char* args) {
  char loggingLevel[5];
  getArg(args, loggingLevel, 0);
  int loggingLevelInt = atoi(loggingLevel);
  if (loggingLevelInt < 0 || loggingLevelInt > 5) {
    return;
  }
  i2c_cfg->loggingLevel = loggingLevelInt;
}

void handleGetSclLevel(I2C_Config* i2c_cfg, const char* args){
  char sclLevel[5];
  itoa(HAL_pinRead(i2c_cfg->sclInPin), sclLevel, 10);
  console_print(sclLevel);
  console_print(NEW_LINE);
}

void handleGetSdaLevel(I2C_Config* i2c_cfg, const char* args){
  char sdaLevel[5];
  itoa(HAL_pinRead(i2c_cfg->sdaInPin), sdaLevel, 10);
  console_print(sdaLevel);
  console_print(NEW_LINE);
}

void handleSetScl(I2C_Config* i2c_cfg, const char* args){
  char sclLevel[5];
  itoa(HAL_pinRead(i2c_cfg->sdaInPin), sclLevel, 10);
  HAL_pinWrite(i2c_cfg->sclOutPin, sclLevel == 0 ? LOW : HIGH);
}

void handleSetSda(I2C_Config* i2c_cfg, const char* args){
  char sdaLevel[5];
  itoa(HAL_pinRead(i2c_cfg->sdaInPin), sdaLevel, 10);
  HAL_pinWrite(i2c_cfg->sclOutPin, sdaLevel == 0 ? LOW : HIGH);
}

static Command getCommandTable[] = {
  {"role", handleGetRole},
  {"address", handleGetAddress},
  {"time_unit", handleGetTimeUnit},
  {"logging_level", handleGetLoggingLevel},
  {"scl", handleGetSclLevel},
  {"sda", handleGetSdaLevel}
};

static Command setCommandTable[] = {
  {"role", handleSetRole},
  {"address", handleSetAddr},
  {"time_unit", handleSetTimeUnit},
  {"logging_level", handleSetLoggingLever},
  {"scl", handleSetScl},
  {"sda", handleSetSda}
};

void handleWriteByte(I2C_Config* i2c_cfg, const char* args) {
  char byte[5];
  getArg(args, byte, 0);
  console_print("B:");
  console_print(byte);
  console_print(NEW_LINE);
  int byteInt = atoi(byte);
  if (byteInt < 0 || byteInt > 255) {
    return;
  }

  I2C_write(i2c_cfg, byteInt);
}

void handleWriteAddress(I2C_Config* i2c_cfg, const char* args) {
  char address[5];
  char direction[5];
  getArg(args, address, 0);
  getArg(args, direction, 1);

  int addressInt = atoi(address);
  if (addressInt < 0 || addressInt > 127) {
    return;
  }

  if (strcmp(direction, "r") == 0) {
    I2C_writeAddress(i2c_cfg, addressInt, READ);
  } else if (strcmp(direction, "w") == 0) {
    I2C_writeAddress(i2c_cfg, addressInt, WRITE);
  }
}

void handleWriteStartCondition(I2C_Config* i2c_cfg, const char* args) {
  I2C_sendStartCondition(i2c_cfg);
}

void handleWriteStopCondition(I2C_Config* i2c_cfg, const char* args) {
  I2C_sendStopCondition(i2c_cfg);
}

void handleWriteToCondition(I2C_Config* i2c_cfg, const char* args) {
  char address[5];
  char payload[5];
  getArg(args, address, 0);
  getArg(args, payload, 1);
  int addressInt = atoi(address);
  int payloadInt = atoi(payload);

  if (addressInt < 0 || addressInt > 127) {
    return;
  }

  I2C_sendStartCondition(i2c_cfg);
  bool ack = I2C_writeAddress(i2c_cfg, addressInt, WRITE);
  if(ack){
    I2C_write(i2c_cfg, payloadInt);
  }

  I2C_sendStopCondition(i2c_cfg);
}

static Command writeCommandTable[] = {
  {"byte", handleWriteByte},
  {"addr", handleWriteAddress},
  {"start", handleWriteStartCondition},
  {"stop", handleWriteStopCondition},
  {"to", handleWriteToCondition}
};

void console_init(void (*print)(char*)){
  console_print = print;
}

void console_parse(I2C_Config* cfg, const char* instruction) {
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
      commandTable[i].handler(cfg, instruction + strlen(command) + 2);
      // commandTable[i].handler(cfg, instruction + strlen(command) + strlen(arg1) + 2);
#else
      commandTable[i].handler(cfg, instruction + strlen(command) + strlen(arg1) + 2);
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