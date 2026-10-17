#include "neomatrix.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"   // generated from ws2812.pio by CMake

// Hardware connections (Feather RP2040 + Propmaker FeatherWing)
#define NEOPIXEL_DATA_PIN   7    // Propmaker "NEO" = Feather D5 = GPIO7
#define NEOPIXEL_POWER_PIN  10   // Propmaker "PWR" = Feather D10 = GPIO10
#define WS2812_FREQ         800000

NeoMatrix::NeoMatrix(uint8_t width, uint8_t height)
    : width(width), height(height), pixels(width * height, 0),
      pio(pio0), sm(0) {}

bool NeoMatrix::init(void) {
    // The Propmaker keeps NeoPixel power off until this pin is driven high
    gpio_init(NEOPIXEL_POWER_PIN);
    gpio_set_dir(NEOPIXEL_POWER_PIN, GPIO_OUT);
    gpio_put(NEOPIXEL_POWER_PIN, 1);

    // Load the WS2812 program into PIO0 and grab a free state machine
    if (!pio_can_add_program(pio, &ws2812_program)) {
        return false;
    }
    uint offset = pio_add_program(pio, &ws2812_program);

    int claimed = pio_claim_unused_sm(pio, false);
    if (claimed < 0) {
        return false;
    }
    sm = (uint)claimed;

    // false = RGB (24-bit) LEDs, not RGBW
    ws2812_program_init(pio, sm, offset, NEOPIXEL_DATA_PIN, WS2812_FREQ, false);

    // Start with all LEDs off
    clear_pixels();
    write();
    return true;
}

void NeoMatrix::set_pixel(uint8_t row, uint8_t col, uint32_t color) {
    if (row >= height || col >= width) {
        return;
    }
    pixels[row * width + col] = color;
}

void NeoMatrix::clear_pixels(void) {
    for (size_t i = 0; i < pixels.size(); i++) {
        pixels[i] = 0x00;
    }
}

void NeoMatrix::write(void) {
    for (size_t i = 0; i < pixels.size(); i++) {
        uint32_t c = pixels[i];
        uint8_t r = (c >> 16) & 0xFF;
        uint8_t g = (c >> 8) & 0xFF;
        uint8_t b = c & 0xFF;

        // WS2812 wants green, red, blue order. The PIO program shifts
        // bits out from the top of the word, so put the 24 bits in the
        // upper part of the 32-bit word.
        uint32_t grb = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
        pio_sm_put_blocking(pio, sm, grb << 8u);
    }

    // Wait for the FIFO to drain, then hold the line low long enough
    // (> 280 us) for the LEDs to latch the new colors.
    while (!pio_sm_is_tx_fifo_empty(pio, sm)) {
        tight_loop_contents();
    }
    sleep_us(300);
}
