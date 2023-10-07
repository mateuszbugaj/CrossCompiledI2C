```
cmake -S . -B build/ -DTARGET_PLATFORM=AVR -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake
cmake --build build-avr/
sudo make -C build-avr/ upload_AvrDemo
```

```
cmake -S . -B build-desktop/ -DTARGET_PLATFORM=DESKTOP
cmake --build build-desktop/
build-desktop/src/desktop/DesktopDemo
```