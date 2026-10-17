// Bubble level demo: uses the LIS3DH driver and the NeoMatrix driver.
//  - Level (|x| and |y| within 0.1 g): center 4 LEDs solid green.
//  - Tilted: a single red LED shows which way the board is tilted.

#include <stdio.h>
#include "pico/stdlib.h"

#include "lis3dh.h"
#include "neomatrix.h"

#define MATRIX_SIZE     8
#define LEVEL_THRESHOLD 0.1f      // g
#define GREEN           0x002000  // dimmed so it's not blinding
#define RED             0x200000

// Map an acceleration in [-1 g, +1 g] to an LED index 0..7.
static uint8_t accel_to_index(float a) {
    int i = (int)((a + 1.0f) * (MATRIX_SIZE / 2.0f));
    if (i < 0) i = 0;
    if (i > MATRIX_SIZE - 1) i = MATRIX_SIZE - 1;
    return (uint8_t)i;
}

int main() {
    stdio_init_all();

    LIS3DH accel;
    NeoMatrix matrix(MATRIX_SIZE, MATRIX_SIZE);

    if (!matrix.init()) {
        printf("NeoMatrix init failed\n");
    }

    // If the accelerometer is missing, light the whole matrix red and
    // keep retrying so the problem is obvious.
    while (!accel.init()) {
        printf("LIS3DH init failed, retrying...\n");
        for (uint8_t r = 0; r < MATRIX_SIZE; r++)
            for (uint8_t c = 0; c < MATRIX_SIZE; c++)
                matrix.set_pixel(r, c, RED);
        matrix.write();
        sleep_ms(1000);
    }

    while (true) {
        accel.update();
        printf("x: %6.3f g  y: %6.3f g  z: %6.3f g\n", accel.x, accel.y, accel.z);

        matrix.clear_pixels();

        bool level = (accel.x > -LEVEL_THRESHOLD && accel.x < LEVEL_THRESHOLD &&
                      accel.y > -LEVEL_THRESHOLD && accel.y < LEVEL_THRESHOLD);

        if (level) {
            // Center 4 LEDs
            matrix.set_pixel(3, 3, GREEN);
            matrix.set_pixel(3, 4, GREEN);
            matrix.set_pixel(4, 3, GREEN);
            matrix.set_pixel(4, 4, GREEN);
        } else {
            // x tilt moves the dot left/right, y tilt moves it up/down.
            // If the dot moves opposite to what you expect, negate x or y here.
            uint8_t col = accel_to_index(accel.x);
            uint8_t row = accel_to_index(accel.y);
            matrix.set_pixel(row, col, RED);
        }

        matrix.write();
        sleep_ms(50);
    }
}
