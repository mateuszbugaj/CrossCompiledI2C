add_executable(DesktopDemo DesktopDemo.cpp)
target_include_directories(DesktopDemo PUBLIC .)
target_link_libraries(DesktopDemo GenericI2C Console)