#include <gtest/gtest.h>

extern "C" {
  #include <I2C_HAL.h>
  #include <I2C_HAL_DESKTOP.h>
}

class I2C_HALTest : public testing::Test {
  protected:
    HAL_Pin* sclOutPin;
    HAL_Pin* sclInPin;

    void SetUp() override {
      I2C_HAL_init();
      sclOutPin = HAL_pinSetup(HAL_PinRole::SCL_OUT);
      sclInPin = HAL_pinSetup(HAL_PinRole::SCL_IN);
    }

    void TearDown() override {
      delete sclOutPin;
      delete sclInPin;
    }
};

TEST_F(I2C_HALTest, PinInit){
  EXPECT_EQ(sclOutPin->pinRole, HAL_PinRole::SCL_OUT);
  EXPECT_EQ(sclOutPin->level, HAL_PinLevel::LOW);
  EXPECT_EQ(sclOutPin->direction, HAL_PinDirection::OUTPUT);

  EXPECT_EQ(sclInPin->pinRole, HAL_PinRole::SCL_IN);
  EXPECT_EQ(sclInPin->level, HAL_PinLevel::LOW);
  EXPECT_EQ(sclInPin->direction, HAL_PinDirection::INPUT);
}

/*
Set SCL of the I2C bus to low by setting SCL of the device (SCL_OUT) to HIGH.
The SCL_OUT is in real-life implementation driving the transistor in the 
open-collector configuration and thus, it's level needs to be inverted.

Check if the level value is correctly changed using SCL_IN pin.
*/
TEST_F(I2C_HALTest, SetSclLow){
  /* Read SCL */
  HAL_PinLevel sclLevel = HAL_pinRead(sclInPin);
  EXPECT_EQ(HAL_PinLevel::HIGH, sclLevel);
  
  /* Write HIGH to SCL_OUT pin */
  HAL_pinWrite(sclOutPin, HAL_PinLevel::HIGH);

  /* Read SCL again */
  sclLevel = HAL_pinRead(sclInPin);
  EXPECT_EQ(HAL_PinLevel::LOW, sclLevel);

  /* Write LOW to SCL_OUT pin */
  HAL_pinWrite(sclOutPin, HAL_PinLevel::LOW);

  /* Read SCL to check if it's back to HIGH */
  sclLevel = HAL_pinRead(sclInPin);
  EXPECT_EQ(HAL_PinLevel::HIGH, sclLevel);
}

/*
Create additional SCL_OUT pin that will also drive the SCL of the bus.
Check if two pins are successfully acting in AND configuration.

| sclOutPin | sclOutPin2 | SCL  |
| LOW       | LOW        | HIGH |
| HIGH      | LOW        | LOW  |
| LOW       | HIGH       | LOW  |
| HIGH      | HIGH       | LOW  |

*/
TEST_F(I2C_HALTest, twoSclPinsInAndConfiguration){
  HAL_Pin* sclOutPin2 = HAL_pinSetup(HAL_PinRole::SCL_OUT);

  /* LOW + LOW = HIGH */
  HAL_pinWrite(sclOutPin, HAL_PinLevel::LOW);
  HAL_pinWrite(sclOutPin2, HAL_PinLevel::LOW);
  EXPECT_EQ(HAL_PinLevel::HIGH, HAL_pinRead(sclInPin));

  /* HIGH + LOW = LOW */
  HAL_pinWrite(sclOutPin, HAL_PinLevel::HIGH);
  HAL_pinWrite(sclOutPin2, HAL_PinLevel::LOW);
  EXPECT_EQ(HAL_PinLevel::LOW, HAL_pinRead(sclInPin));

  /* LOW + HIGH = LOW */
  HAL_pinWrite(sclOutPin, HAL_PinLevel::LOW);
  HAL_pinWrite(sclOutPin2, HAL_PinLevel::HIGH);
  EXPECT_EQ(HAL_PinLevel::LOW, HAL_pinRead(sclInPin));

  /* HIGH + HIGH = LOW */
  HAL_pinWrite(sclOutPin, HAL_PinLevel::HIGH);
  HAL_pinWrite(sclOutPin2, HAL_PinLevel::HIGH);
  EXPECT_EQ(HAL_PinLevel::LOW, HAL_pinRead(sclInPin));

  delete sclOutPin2;
}