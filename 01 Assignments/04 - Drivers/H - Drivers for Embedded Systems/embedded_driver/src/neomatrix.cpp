#include "neomatrix.h" // Includes the NeoMatrix header file
#include "ws2812.pio.h" // Includes the PIO program header for WS2812 LED control
#include "pico/stdlib.h" // Includes core Pico standard library functions and timing macros
#include <string.h> // Includes standard C string/memory manipulation functions
#include "ws2812.pio.h"   // generated from ws2812.pio by CMake // Secondary inclusion of the generated PIO header file

#define NEOPIXEL_PIN 7 // Propmaker "NEO" = Feather D5 = GPIO07 // Defines the GPIO pin connected to the NeoPixel data line (GPIO 7)
#define NEOPIXEL_POWER_PIN 10  // Propmaker "PWR" = FEATHER D10 = GPIO10 // Defines the GPIO pin controlling power to the Propmaker NeoPixels (GPIO 10)
#define WS2812_FREQ         800000 // Defines the target data frequency for WS2812 LEDs (800 kHz)

NeoMatrix::NeoMatrix(uint8_t width, uint8_t height) // Constructor definition taking matrix width and height
    : width(width), // Initializes the width member variable with the constructor parameter
    height(height), // Initializes the height member variable with the constructor parameter
    pixels(width * height, 0), // Initializes the vector storage for pixels with size width*height filled with 0
    pio(pio0), // Initializes the PIO hardware block pointer to pio0
    sm(0), // Initializes the state machine index to 0
    offset(0) {} // Initializes the PIO program memory offset to 0

bool NeoMatrix::init(void) { // Initialization method for configuring power and setting up the PIO state machine
    // Enable power pin for Propmaker FeatherWing NeoPixels
    // Keeps NeoPixel power off until this pin is driven high
    gpio_init(NEOPIXEL_POWER_PIN); // Initializes the NeoPixel power control GPIO pin
    gpio_set_dir(NEOPIXEL_POWER_PIN, GPIO_OUT); // Sets the power pin direction as an output
    gpio_put(NEOPIXEL_POWER_PIN, 1); // Drives the power pin high to turn on power to the NeoMatrix LEDs
    sleep_ms(10); // Pauses for 10 milliseconds to allow power rail voltages to stabilize

    // Loads WS2812 program into PIO0 and grab a free state
    if (!pio_can_add_program(pio, &ws2812_program)) { // Checks if there is enough space in PIO memory to load the WS2812 program
        return false; // Returns false if the program cannot be added to the PIO block
    }
    offset = pio_add_program(pio, &ws2812_program); // Loads the WS2812 program into PIO instruction memory and saves its offset

    int claimed = pio_claim_unused_sm(pio, false); // Attempts to claim an unused state machine on the PIO block without panicking
    if (claimed < 0) { // Checks if claiming the state machine failed (returned negative value)
        return false; // Returns false if no free state machine is available
    }
    sm = (uint)claimed; // Casts and saves the claimed state machine index number

    // RGB leds, not RGBW
    ws2812_program_init(pio, sm, offset, NEOPIXEL_PIN, WS2812_FREQ, false); // Initializes the PIO state machine with the WS2812 driver settings

    clear_pixels(); // Clears the pixel buffer data (turns all pixels off in memory)
    write(); // Flushes the cleared pixel buffer out to the physical LEDs to ensure a clean startup state
    return true; // Returns true indicating successful matrix initialization
}

void NeoMatrix::set_pixel(uint8_t row, uint8_t col, uint32_t color) { // Method to set a specific pixel's color given its row and column
    if (row >= height || col >= width) return; // Bounds check: returns early if row or column exceeds matrix dimensions
    // Account for serpentine matrix wiring
    uint8_t actual_col = (row % 2 == 1) ? (width - 1 - col) : col; // Adjusts column index for serpentine-wired matrix layouts on odd rows
    pixels[row * width + actual_col] = color; // Stores the color value into the 1D vector at the calculated 2D grid index
}

void NeoMatrix::clear_pixels(void) { // Method to clear all pixels in the frame buffer
    for (size_t i = 0; i < pixels.size(); i++) { // Loops through every element in the pixels vector
        pixels[i] = 0x00; // Sets each pixel color value to 0 (off)
    }
}

void NeoMatrix::write(void) { // Method to transmit the pixel buffer to the physical LED hardware via PIO
    for (size_t i = 0; i < pixels.size(); i++) { // Iterates through each pixel in the buffer
        uint32_t color = pixels[i]; // Retrieves the 32-bit color value for the current pixel
        uint8_t r = (color >> 16) & 0xFF; // Extracts the 8-bit red component from bits [23:16]
        uint8_t g = (color >> 8) & 0xFF; // Extracts the 8-bit green component from bits [15:8]
        uint8_t b = color & 0xFF; // Extracts the 8-bit blue component from bits [7:0]
        // WS2812 expects GRB byte ordering
        uint32_t grb = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b; // Rearranges RGB components into GRB byte order expected by WS2812 LEDs
        pio_sm_put_blocking(pio, sm, grb << 8); // Pushes the shifted GRB color word into the PIO state machine transmit FIFO blocking until space is free
    }
    sleep_us(300); // Latch duration // Pauses for 300 microseconds to allow LEDs to latch the data frame
}