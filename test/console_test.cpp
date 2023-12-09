#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>
#include <vector>

extern "C" {
  #include <console.h>
}

static std::vector<std::string> expectedStrs;
class ConsoleTest : public testing::Test {
  protected:
    I2C_Config i2c_config;

    static void testPrintFunction(char* str) {
      expectedStrs.push_back(str);
    }

    void SetUp() override {
      console_init(ConsoleTest::testPrintFunction);
    }

    void TearDown() override {
      expectedStrs.clear();
    }
};


TEST_F(ConsoleTest, ConsoleParseGetRole) {
  I2C_Role expectedRole = I2C_Role::MASTER;

  i2c_config.role = expectedRole;
  console_parse(&i2c_config, "get role");

  EXPECT_STREQ(expectedStrs[0].c_str(), "MASTER");
}

TEST_F(ConsoleTest, ConsoleParseGetAddress) {
  int expectedAddress = 51;

  i2c_config.addr = expectedAddress;
  console_parse(&i2c_config, "get address");

  EXPECT_STREQ(expectedStrs[0].c_str(), std::to_string(expectedAddress).c_str());
}

TEST_F(ConsoleTest, ConsoleParseGetTimeUnit) {
  int expectedTimeUnit = 100;

  i2c_config.timeUnit = expectedTimeUnit;
  console_parse(&i2c_config, "get time_unit");

  EXPECT_STREQ(expectedStrs[0].c_str(), std::to_string(expectedTimeUnit).c_str());
}

TEST_F(ConsoleTest, ConsoleParseGetLoggingLevel) {
  int expectedLoggingLevel = 100;

  i2c_config.loggingLevel = expectedLoggingLevel;
  console_parse(&i2c_config, "get logging_level");

  EXPECT_STREQ(expectedStrs[0].c_str(), std::to_string(expectedLoggingLevel).c_str());
}

TEST_F(ConsoleTest, ConsoleParseSetRole) {
  I2C_Role oldRole = I2C_Role::MASTER;
  I2C_Role newRole = I2C_Role::SLAVE;

  i2c_config.role = oldRole;
  EXPECT_EQ(i2c_config.role, oldRole);

  std::string cmd = "set role " + std::string(newRole == I2C_Role::MASTER ? "master" : "slave");
  console_parse(&i2c_config, cmd.c_str());
  EXPECT_EQ(i2c_config.role, newRole);
}

TEST_F(ConsoleTest, ConsoleParseSetAddress) {
  int oldAddress = 51;
  int newAddress = 84;

  i2c_config.addr = oldAddress;
  EXPECT_EQ(i2c_config.addr, oldAddress);

  std::string cmd = "set address " + std::to_string(newAddress);
  console_parse(&i2c_config, cmd.c_str());
  EXPECT_EQ(i2c_config.addr, newAddress);
}

TEST_F(ConsoleTest, ConsoleParseSetTimeUnit) {
  int oldTimeUnit = 100;
  int newTimeUnit = 200;

  i2c_config.timeUnit = oldTimeUnit;
  EXPECT_EQ(i2c_config.timeUnit, oldTimeUnit);

  std::string cmd = "set time_unit " + std::to_string(newTimeUnit);
  console_parse(&i2c_config, cmd.c_str());
  EXPECT_EQ(i2c_config.timeUnit, newTimeUnit);
}

TEST_F(ConsoleTest, ConsoleParseSetLoggingLevel) {
  int oldLoggingLevel = 4;
  int newLoggingLevel = 5;

  i2c_config.loggingLevel = oldLoggingLevel;
  EXPECT_EQ(i2c_config.loggingLevel, oldLoggingLevel);

  std::string cmd = "set logging_level " + std::to_string(newLoggingLevel);
  console_parse(&i2c_config, cmd.c_str());
  EXPECT_EQ(i2c_config.loggingLevel, newLoggingLevel);
}