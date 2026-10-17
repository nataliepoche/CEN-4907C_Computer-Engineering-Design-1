cmake_minimum_required(VERSION 3.13)

# Build for the Adafruit Feather RP2040 (must be set before the SDK is imported)
set(PICO_BOARD adafruit_feather_rp2040)

# Pull in the Pico SDK (uses the PICO_SDK_PATH environment variable)
include(pico_sdk_import.cmake)

project(embedded_driver C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# ---------- LIS3DH accelerometer driver library ----------
add_library(lis3dh STATIC src/lis3dh.cpp)
target_include_directories(lis3dh PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(lis3dh PUBLIC pico_stdlib hardware_i2c)

# ---------- NeoMatrix driver library ----------
add_library(neomatrix STATIC src/neomatrix.cpp)
# Compile ws2812.pio into ws2812.pio.h, which neomatrix.cpp includes
pico_generate_pio_header(neomatrix ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio)
target_include_directories(neomatrix PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(neomatrix PUBLIC pico_stdlib hardware_pio)

# ---------- Bubble level application ----------
add_executable(level src/level.cpp)
target_link_libraries(level lis3dh neomatrix pico_stdlib)

# printf goes over USB serial, not UART
pico_enable_stdio_usb(level 1)
pico_enable_stdio_uart(level 0)

# Creates level.uf2 (plus .elf, .bin, .hex, .map) in the build directory
pico_add_extra_outputs(level)
