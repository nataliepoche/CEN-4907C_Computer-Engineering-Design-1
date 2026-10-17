#ifndef NEOMATRIX_H
#define NEOMATRIX_H

#include <stdint.h>
#include <vector>

#include "hardware/pio.h"

// Driver for the 8x8 NeoMatrix (WS2812 RGB LEDs), plugged into the
// Propmaker FeatherWing's NeoPixel port.
// Colors are given as 0xRRGGBB (e.g. 0xFF0000 = red).
class NeoMatrix {
public:
    NeoMatrix(uint8_t width, uint8_t height);

    // Powers the NeoPixel port and starts the PIO state machine.
    // Returns false if no PIO state machine / program space is free.
    bool init(void);

    // Set one pixel in the buffer (row and col are zero-indexed).
    // Out-of-range positions are ignored.
    void set_pixel(uint8_t row, uint8_t col, uint32_t color);

    // Set every pixel in the buffer to 0x00 (off).
    void clear_pixels(void);

    // Send the whole buffer to the LEDs.
    void write(void);

private:
    uint8_t width;
    uint8_t height;
    std::vector<uint32_t> pixels;   // row-major, width * height entries

    PIO pio;
    uint sm;
};

#endif
