#include <stdbool.h>
#include <string.h>

#include <SPI_console.h>
#include <itoa.h>

#ifdef DESKTOP
#define NEW_LINE "\n"
#else
#define NEW_LINE "\n\r"
#endif

void (*console_print)(char*) = 0;
typedef void (*CommandHandler)(SPI_Config* cfg, const char*);

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

static void handleGetRole(SPI_Config* cfg,  char* args) {
  console_print(cfg->role == SPI_MASTER ? "MASTER" : "SLAVE");
  console_print(NEW_LINE);
}

static void handleSetRole(SPI_Config* cfg, const char* args) {
  char role[10];
  getArg(args, role, 0);
  if (strcmp(role, "master") == 0) {
    cfg->role = SPI_MASTER;
    cfg->SPICR1 |= (1 << SPI_MSTR);
  } else if (strcmp(role, "slave") == 0) {
    cfg->role = SPI_SLAVE;
    cfg->SPICR1 &= ~(1 << SPI_MSTR);
  }
}

static void handleGetBitInterval(SPI_Config* cfg, const char* args) {
  char bitInterval[5];
  itoa(cfg->SPIBR, bitInterval, 10);
  console_print(bitInterval);
  console_print(NEW_LINE);
}

static void handleSetBitInterval(SPI_Config* cfg, const char* args) {
  char bitInterval[5];
  getArg(args, bitInterval, 0);
  cfg->SPIBR = atoi(bitInterval);
}

static void handleGetClockPolarity(SPI_Config* cfg, const char* args) {
  console_print(bitCheck(cfg->SPICR1, SPI_CPOL) ? "1" : "0");
  console_print(NEW_LINE);
}

static void handleSetClockPolarity(SPI_Config* cfg, const char* args) {
  char clockPolarity[5];
  getArg(args, clockPolarity, 0);
  if (strcmp(clockPolarity, "1") == 0) {
    cfg->SPICR1 |= (1 << SPI_CPOL);
  } else if (strcmp(clockPolarity, "0") == 0) {
    cfg->SPICR1 &= ~(1 << SPI_CPOL);
  }
}

static void handleGetClockPhase(SPI_Config* cfg, const char* args) {
  console_print(bitCheck(cfg->SPICR1, SPI_CPHA) ? "1" : "0");
  console_print(NEW_LINE);
}

static void handleSetClockPhase(SPI_Config* cfg, const char* args) {
  char clockPhase[5];
  getArg(args, clockPhase, 0);
  if (strcmp(clockPhase, "1") == 0) {
    cfg->SPICR1 |= (1 << SPI_CPHA);
  } else if (strcmp(clockPhase, "0") == 0) {
    cfg->SPICR1 &= ~(1 << SPI_CPHA);
  }
}

static void handleGetSSLevel(SPI_Config* cfg, const char* args) {
  console_print(cfg->ssLevel == SPI_HIGH ? "1" : "0");
  console_print(NEW_LINE);
}

static void handleSetSSLevel(SPI_Config* cfg, const char* args) {
  char ssLevel[5];
  getArg(args, ssLevel, 0);
  if (strcmp(ssLevel, "1") == 0) {
    cfg->ssLevel = SPI_HIGH;
    SPI_HAL_pinWrite(cfg->SS, SPI_HIGH);
  } else if (strcmp(ssLevel, "0") == 0) {
    cfg->ssLevel = SPI_LOW;
    SPI_HAL_pinWrite(cfg->SS, SPI_LOW);
  }
}

static void handleGetSPIDR(SPI_Config* cfg, const char* args) {
  char SPIDR[5];
  itoa(cfg->SPIDR, SPIDR, 10);
  console_print(SPIDR);
  console_print(NEW_LINE);
}

static Command getCommandTable[] = {
  {"role", handleGetRole},
  {"bitInterval", handleGetBitInterval},
  {"clockPolarity", handleGetClockPolarity},
  {"clockPhase", handleGetClockPhase},
  {"ssLevel", handleGetSSLevel},
  {"SPIDR", handleGetSPIDR}
};

static Command setCommandTable[] = {
  {"role", handleSetRole},
  {"bitInterval", handleSetBitInterval},
  {"clockPolarity", handleSetClockPolarity},
  {"clockPhase", handleSetClockPhase},
  {"ssLevel", handleSetSSLevel}
};

static void handleWriteByte(SPI_Config* cfg, const char* args) {
  char byte[5];
  char ss[5];
  getArg(args, ss, 0);
  getArg(args, byte, 1);
  SPI_send(cfg, atoi(byte), SPI_getSSPin(cfg, atoi(ss)));
}

static Command writeCommandTable[] = {
  {"byte", handleWriteByte}
};

void common_SPI_consoleInit(void (*print)(char*)){
  console_print = print;
}

void common_SPI_consoleParse(SPI_Config* cfg, const char* instruction) {
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