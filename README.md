# Cross compiled I2C
A demo/development platform for I2C protocol. </br>
It can be run on a desktop by starting two instances of the executable that communicate with each other using a JSON file or it can be build and uploaded to run on the AVR/STM32 microcontrollers. </br>
Each target platform is using the same version of the [I2C library](https://github.com/mateuszbugaj/GenericI2C) by compiling the appropriate HAL implementation and has a common console source code for control.

There are scripts for each target platforms that help with deployment under `utils/`. </br>
The [logic analyzer](https://github.com/mateuszbugaj/SimpleLogicAnalyzer) can be used to examine the transmission.
## Desktop platform
Create the build-system
```
cmake -S . -B build-desktop/ -DTARGET_PLATFORM=DESKTOP
```

Build executable
```
cmake --build build-desktop/
```

Run `transmitter` instance
```
build-desktop/src/desktop/DesktopDemo --role=1
```

Run `receiver` instance
```
build-desktop/src/desktop/DesktopDemo --role=2
```

The HAL implementation is using `output/pin_states.json` instead of real MCU pins to read and write to the I2C bus. </br>
Each new pin output generates new line which contains the state of pins and the calculated bus state.
```JSON
{"timestamp":"16:02:59.415","signals":[{"MASTER_SCL_OUT":0},{"MASTER_SDA_OUT":0},{"SLAVE_SCL_OUT":0},{"SLAVE_SDA_OUT":0}],"SCL":1,"SDA":1}
```

Other files in the `output/` directory such as `logic_analyzer_snapshot.txt`, `master_log.txt` or `slave_log.txt` can be used by [logic analyzer](https://github.com/mateuszbugaj/SimpleLogicAnalyzer) by providing the filenames in the analyzer-properties file.
```json
{
  "logicProbe": { "file": "/path/to/the/logic_analyzer_snapshot.txt" },
  "loggingProbe": [
    { "name": "Transceiver", "file": "/path/to/the/master_log.txt" },
    { "name": "Receiver", "file": "/path/to/the/slave_log.txt"}
  ],
  "signals": ["SDA", "SCL", "Transmitter SDA", "Transmitter SCL", "Receiver SDA", "Receiver SCL"]
}
```


## AVR platform

```
cmake -S . -B build-avr/ -DTARGET_PLATFORM=AVR -DCMAKE_TOOLCHAIN_FILE=cmake/avr/avr-toolchain.cmake
cmake --build build-avr/
sudo make -C build-avr/ upload_AvrDemo
```

## STM32 platform
```
cmake -S . -B build-stm32/ -DTARGET_PLATFORM=STM32 -DCMAKE_TOOLCHAIN_FILE=cmake/stm32/stm32-toolchain.cmake
cmake --build build-stm32/
sudo make -C build-stm32/ Stm32Demo.bin_upload
```
