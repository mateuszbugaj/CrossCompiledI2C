add_subdirectory(googletest)
enable_testing()

add_executable(Tests console_test.cpp I2C_HAL_Desktop_Impl_test.cpp)
target_include_directories(Tests PUBLIC .)
target_link_libraries(Tests gtest_main Console GenericI2C)