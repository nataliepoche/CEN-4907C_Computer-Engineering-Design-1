# embedded_driver — LIS3DH + NeoMatrix drivers and Bubble Level

Ethan Ruddell — CEN3907C

Two C++ driver libraries for the CEN3907C hardware kit (Adafruit Feather RP2040 +
Propmaker FeatherWing + 8x8 NeoMatrix), and a bubble-level demo app that uses them.

## Project structure

```
embedded_driver/
├── src/
│   ├── lis3dh.h / lis3dh.cpp        LIS3DH accelerometer driver (I2C)
│   ├── neomatrix.h / neomatrix.cpp  NeoMatrix display driver (PIO, WS2812)
│   ├── ws2812.pio                   WS2812 PIO program (from pico-examples)
│   └── level.cpp                    Bubble level application
├── CMakeLists.txt
├── pico_sdk_import.cmake
└── README.md
```

`CMakeLists.txt` builds each driver as its own static library (`lis3dh`, `neomatrix`)
and links both into the `level` executable. `ws2812.pio` is compiled into
`ws2812.pio.h` by `pico_generate_pio_header`, which the NeoMatrix driver includes.
`PICO_BOARD` is set to `adafruit_feather_rp2040` inside `CMakeLists.txt`, so no
extra cmake flags are needed.

## Hardware connections used

| Function | Feather pin | RP2040 GPIO | Notes |
|---|---|---|---|
| LIS3DH SDA | SDA | GPIO2 | I2C1 (only I2C instance available on these pins) |
| LIS3DH SCL | SCL | GPIO3 | I2C address 0x18 |
| NeoPixel data | D5 | GPIO7 | Driven by PIO0 |
| NeoPixel power enable | D10 | GPIO10 | Must be driven high; off by default |

## Driver interfaces

**LIS3DH** — `init()` sets up I2C1 at 400 kHz, checks `WHO_AM_I` (0x33), then enables
all axes at 1.344 kHz, ±2 g. `update()` refreshes the public `x`, `y`, `z` members in g.
`set_reg()` / `read_reg()` give raw single-register access.

**NeoMatrix** — `NeoMatrix(width, height)` allocates a row-major pixel buffer.
`init()` powers the NeoPixel port and starts a PIO state machine. `set_pixel(row, col,
color)` takes colors as `0xRRGGBB`; the driver reorders to GRB when writing.
`clear_pixels()` zeros the buffer and `write()` sends it out.

## Bubble level behavior

- |x| and |y| both under 0.1 g → center four LEDs solid green.
- Otherwise → one red LED; its column follows x and its row follows y, so its
  position shows the direction of tilt.
- Accelerometer readings are also printed over USB serial.

## Building

Requires the Pico SDK and the ARM toolchain (Linux or WSL2).

```bash
export PICO_SDK_PATH=/path/to/pico-sdk
cd embedded_driver
mkdir build && cd build
cmake ..
make
```

This produces `build/level.uf2`.

## Loading

1. Plug the Feather into USB.
2. Hold **BOOTSEL**, press and release **RESET**, then release BOOTSEL.
   The board mounts as a drive named `RPI-RP2`.
3. Drag `build/level.uf2` onto that drive. The board reboots and runs the level.

To watch the serial output: `minicom -D /dev/ttyACM0` (Linux) or PuTTY on the COM port (Windows).
