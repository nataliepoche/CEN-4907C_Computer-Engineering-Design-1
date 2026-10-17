# RP2040 Embedded Drivers & Bubble Level Application

Developed for **CEN3907C (Computer Engineering)** at the **University of Florida**, this project implements modular C++ hardware drivers for an **LIS3DH I2C Accelerometer** and an **8x8 WS2812 NeoMatrix** display on the **Adafruit Feather RP2040** (paired with an Adafruit Propmaker FeatherWing). The project culminates in an interactive "Bubble Level" application that visualizes tilt dynamics using the LED matrix.

---

## Hardware Architecture & Pinout

* **Microcontroller:** Adafruit Feather RP2040 (RP2040 dual-arm Cortex-M0+)
* **Accelerometer (LIS3DH):** Connected via I2C1 (`SDA = GPIO2`, `SCL = GPIO3`, Address: `0x18`)
* **LED Matrix (NeoMatrix):** WS2812 RGB Matrix driven via PIO (`NEOPIXEL_PIN = GPIO7`, Power Enable: `GPIO10`)

---

## Project Structure

```text
embedded_driver/
├── CMakeLists.txt         # CMake build configuration and library targets
├── src/
│   ├── level.cpp          # Main application logic (Bubble Level state machine)
│   ├── lis3dh.cpp         # LIS3DH accelerometer driver implementation
│   ├── lis3dh.h           # LIS3DH class definition & register mappings
│   ├── neomatrix.cpp      # NeoMatrix driver (PIO management & serpentine mapping)
│   ├── neomatrix.h        # NeoMatrix class definition & pixel buffer
│   └── ws2812.pio         # PIO assembly program for WS2812 timing protocol
└── README.md              # Project documentation
```

## Instructions

### Initial Build

```
# Create and enter the build directory
mkdir build && cd build

# Configure CMake with automatic SDK fetching enabled
cmake -DPICO_SDK_FETCH_FROM_GIT=ON ..

# Compile the binaries
cmake --build .
```
### Updating after Code Changes

```
# Navigate to the build directory (if not already there)
cd build

# Recompile the project
cmake --build .
```

### Cleaning the Build Directory

```
# From the project root directory, remove the build folder and recreate it
rm -rf build && mkdir build && cd build

# Reconfigure and build
cmake -DPICO_SDK_FETCH_FROM_GIT=ON ..
cmake --build .
```
### Packing Project for Submission
```
tar --exclude="build" -czvf embedded_driver.tgz embedded_driver
```