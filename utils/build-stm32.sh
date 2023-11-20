#!/usr/bin/bash

build_dir="build-stm32"

echo "Building for STM32..."

# If parameter -d is passed, delete the build directory
if [ "$1" == "-d" ]; then
    echo "Deleting build directory..."
    rm -rf ${build_dir}
fi

cmake -S . -B ${build_dir}/ -DTARGET_PLATFORM=STM32 -DCMAKE_TOOLCHAIN_FILE=cmake/stm32/stm32-toolchain.cmake
cmake --build ${build_dir}/

echo "Upload with: make -C ${build_dir}/ Stm32Demo.bin_upload"
make -C ${build_dir}/ Stm32Demo.bin_upload
echo "Done!"