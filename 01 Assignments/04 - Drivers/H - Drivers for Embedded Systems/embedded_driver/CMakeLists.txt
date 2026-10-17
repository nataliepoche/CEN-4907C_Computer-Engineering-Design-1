cmake_minimum_required(VERSION 3.13) # Sets the minimum required version of CMake to 3.13 for building the project

set(PICO_BOARD adafruit_feather_rp2040) # Build for Adafruit Feather PR2040, set before SDK is imported)

include(pico_sdk_import.cmake) # Includes the Pico SDK import script to pull in Raspberry Pi Pico SDK functions

project(embedded_driver C CXX ASM) # Defines the CMake project name as embedded_driver and enables C, CXX, and Assembly languages
set(CMAKE_C_STANDARD 11) # Sets the C standard version to C11
set(CMAKE_CXX_STANDARD 17) # Sets the C++ standard version to C++17

pico_sdk_init() # Initializes the Raspberry Pi Pico SDK environment and build hooks

# add_executable(level # Declares an executable target named level from source files
#     src/level.cpp
#     src/lis3dh.cpp
#     src/neomatrix.cpp
# )

# target_link_libraries(level # Links standard Pico libraries and hardware features to the level target
#     pico_stdlib
#     hardware_i2c
#     hardware_pio
#     hardware_gpio
# )

# pico_enable_stdio_usb(level 1) # Enables standard input/output over USB
# pico_enable_stdio_uart(level 0) # Disables standard input/output over UART

# pico_generate_pio_header(level ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio) # Generates C header from PIO file
# pico_add_extra_outputs(level) # Adds extra binary output formats like uf2, hex, bin

# ---------- LIS3DH accelerometer driver library ----------
add_library(lis3dh STATIC src/lis3dh.cpp) # Creates a static library target named lis3dh compiled from src/lis3dh.cpp
target_include_directories(lis3dh PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src) # Adds the src directory to the include path for the lis3dh library
target_link_libraries(lis3dh PUBLIC pico_stdlib hardware_i2c) # Links pico_stdlib and hardware_i2c libraries to the lis3dh target

# ---------- NeoMatrix driver library ----------
add_library(neomatrix STATIC src/neomatrix.cpp) # Creates a static library target named neomatrix compiled from src/neomatrix.cpp
# Compile ws2812.pio into ws2812.pio.h, which neomatrix.cpp includes
pico_generate_pio_header(neomatrix ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio) # Generates the C header file from the WS2812 PIO program for the neomatrix library
target_include_directories(neomatrix PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src) # Adds the src directory to the include path for the neomatrix library
target_link_libraries(neomatrix PUBLIC pico_stdlib hardware_pio) # Links pico_stdlib and hardware_pio libraries to the neomatrix target

# ---------- Bubble level application ----------
add_executable(level src/level.cpp) # Defines an executable target named level built from src/level.cpp
target_link_libraries(level lis3dh neomatrix pico_stdlib) # Links the custom lis3dh and neomatrix libraries, along with pico_stdlib, to the level executable

# printf goes over USB serial, not UART
pico_enable_stdio_usb(level 1) # Enables standard input/output streaming over USB (stdio via USB CDC)
pico_enable_stdio_uart(level 0) # Disables standard input/output streaming over the hardware UART pins

# Creates level.uf2 (plus .elf, .bin, .hex, .map) in the build directory
pico_add_extra_outputs(level) # Generates UF2 and auxiliary binary file outputs for flashing onto the RP2040 board