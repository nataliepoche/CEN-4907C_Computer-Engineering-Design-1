#ifndef NEOMATRIX_H // Header guard start to prevent multiple inclusions
#define NEOMATRIX_H

#include <stdint.h> // Includes fixed-width integer type definitions
#include <vector> // Includes the C++ Standard Template Library vector container header
#include "hardware/pio.h" // Includes the Raspberry Pi Pico PIO hardware library header

// Colors are 0xRRGGBB (0xFF0000 = red)
class NeoMatrix { // Declares the NeoMatrix LED grid driver class
public:
    NeoMatrix(uint8_t width, uint8_t height); // Constructor declaration accepting matrix width and height
    bool init(void); // Powers NeoPixel and starts PIO state machine, returns false if no PIO state machine // Method declaration for hardware initialization
    void set_pixel(uint8_t row, uint8_t col, uint32_t color); // Method declaration for setting an individual pixel color
    void clear_pixels(void); // Sets every pixel off // Method declaration for turning off all pixels in memory
    void write(void); // Send whole buffer to LEDs // Method declaration for transmitting frame buffer to hardware

private:
    uint8_t width; // Private member variable storing matrix width
    uint8_t height; // Private member variable storing matrix height
    std::vector<uint32_t> pixels;   // row-major, width * height entries // Private vector container storing color data for every pixel
    PIO pio; // Private member variable storing the PIO instance block pointer
    uint sm; // Private member variable storing the PIO state machine identifier number
    uint offset; // Private member variable storing the PIO program memory base offset
};

#endif // NEOMATRIX_H // End of header guard