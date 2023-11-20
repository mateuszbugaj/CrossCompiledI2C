#!/usr/bin/bash

build_dir="build-avr"

echo "Building for AVR..."

# If parameter -d is passed, delete the build directory
if [ "$1" == "-d" ]; then
    echo "Deleting build directory..."
    rm -rf ${build_dir}
fi

cmake -S . -B ${build_dir}/ -DTARGET_PLATFORM=AVR -DCMAKE_TOOLCHAIN_FILE=cmake/avr/avr-toolchain.cmake
cmake --build ${build_dir}/

echo "Upload with: sudo make -C ${build_dir}/ upload_AvrDemo"
sudo make -C ${build_dir}/ upload_AvrDemo
echo "Done!"