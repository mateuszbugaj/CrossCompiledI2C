add_executable(DesktopDemo DesktopDemo.cpp signal_logger.cpp )
target_include_directories(DesktopDemo PUBLIC .)
target_link_libraries(DesktopDemo GenericI2C)