#!/usr/bin/bash

build_dir="build-desktop"

echo "Building for desktop..."

# If parameter -d is passed, delete the build directory
if [ "$1" == "-d" ]; then
    echo "Deleting build directory..."
    rm -rf ${build_dir}
fi

cmake -S . -B ${build_dir}/ -DTARGET_PLATFORM=DESKTOP
cmake --build ${build_dir}/

# If parameter -t is passed, run tests
if [ "$1" == "-t" ]; then
    echo "Running tests..."
    ${build_dir}/test/Tests
fi

echo "Run with: ${build_dir}/src/desktop/DesktopDemo --role=1"
echo "Done!"