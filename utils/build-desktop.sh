#!/usr/bin/bash

build_dir="build-desktop"

echo "Building for desktop..."

program_parameters=()

# Process each argument and the rest of the arguments pass to the program
while [ "$#" -gt 0 ]; do
    case "$1" in
        -d)
            echo "Deleting build directory..."
            rm -rf ${build_dir}
            ;;
        -t)
            test_flag=true
            ;;
        *)
            program_parameters+=("$1")
            ;;
    esac
    shift
done

cmake -S . -B ${build_dir}/ -DTARGET_PLATFORM=DESKTOP
cmake --build ${build_dir}/

# Check if test flag is set and run tests
if [ "${test_flag}" == true ]; then
    echo "Running tests..."
    ${build_dir}/test/Tests
else
    echo "Run: ${build_dir}/src/desktop/DesktopDemo ${program_parameters[@]}"
    # Pass the rest of the arguments to the program
    ${build_dir}/src/desktop/DesktopDemo ${program_parameters[@]}
fi

echo "Done!"