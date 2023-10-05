```
cmake -S . -B build/ -DTARGET_PLATFORM=AVR
cmake --build build/
sudo make -C build/ upload_AvrDemo
```

```
cmake -S . -B build/ -DTARGET_PLATFORM=DESKTOP
cmake --build build/
build/src/desktop/DesktopDemo
```