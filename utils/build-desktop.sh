#!/usr/bin/bash

build_dir="build-desktop"

echo "Building for desktop..."

# Initialize default role value
role="1"

# Process each argument
while [ "$#" -gt 0 ]; do
    case "$1" in
        -d)
            echo "Deleting build directory..."
            rm -rf ${build_dir}
            ;;
        -t)
            test_flag=true
            ;;
        --role)
            if [ -n "$2" ]; then
                role="$2"
                shift
            else
                echo "Error: --role requires an argument."
                exit 1
            fi
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
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
fi

echo "Run with: ${build_dir}/src/desktop/DesktopDemo --role=${role}"
echo "Done!"
${build_dir}/src/desktop/DesktopDemo --role=${role}