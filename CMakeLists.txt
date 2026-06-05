cmake_minimum_required(VERSION 3.20.0)
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/module_drivers/aht10/drivers)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(ble-sensor-node)

target_sources(app PRIVATE app/src/main.cpp)
target_include_directories(app PRIVATE app/src)
add_subdirectory(app/src/ble)