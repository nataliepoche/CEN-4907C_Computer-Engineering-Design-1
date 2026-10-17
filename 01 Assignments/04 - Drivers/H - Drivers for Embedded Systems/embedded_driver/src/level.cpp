#include "pico/stdlib.h" // Includes the Raspberry Pi Pico standard library header for core functions and time delays
#include "lis3dh.h" // Includes the custom header file for the LIS3DH accelerometer driver class
#include "neomatrix.h" // Includes the custom header file for the NeoMatrix LED matrix driver class
#include <stdio.h> // Includes the standard C input/output library for printf debugging
#include <math.h> // Includes the standard C math library for mathematical functions like fabs or sqrt

#define MATRIX_SIZE     8 // Defines a constant for the width and height of the 8x8 LED matrix
#define LEVEL_THRESHOLD 0.1f // Defines the acceleration threshold (in g's) considered to be perfectly level
#define GREEN           0x002000  // dimmed // Defines a dimmed green color value in 0xRRGGBB format
#define RED             0x200000 // Defines a dimmed red color value in 0xRRGGBB format

// Map an acceleration in [-1 g, +1 g] to an LED index 0..7.
static uint8_t accel_to_index(float a) { // Helper function converting an acceleration float value into a matrix coordinate index 0-7
    int i = (int)((a + 1.0f) * (MATRIX_SIZE / 2.0f)); // Scales and shifts the acceleration value from [-1, 1] into a pixel grid index range [0, 8]
    if (i < 0) i = 0; // Clamps the calculated index to a minimum of 0 to prevent out-of-bounds errors
    if (i > MATRIX_SIZE - 1) i = MATRIX_SIZE - 1; // Clamps the calculated index to a maximum of 7 (MATRIX_SIZE - 1)
    return (uint8_t)i; // Casts and returns the final constrained index as an 8-bit unsigned integer
}

int main() { // Main program entry point where execution begins
    stdio_init_all(); // Initializes standard input/output types (enabling USB serial output)
    sleep_ms(1000); // Pauses execution for 1000 milliseconds to allow serial connection to stabilize

    LIS3DH accel; // Instantiates an LIS3DH accelerometer driver object named accel

    NeoMatrix matrix(MATRIX_SIZE, MATRIX_SIZE); // Instantiates a NeoMatrix LED grid driver object with 8x8 dimensions

    if (!matrix.init()) { // Attempts to initialize the NeoMatrix hardware and checks if it failed
        printf("NeoMatrix init failed\n"); // Prints an error message to the serial console if matrix initialization fails
    }

    while (!accel.init()) { // Enters a retry loop that runs until the LIS3DH accelerometer successfully initializes
        printf("Failed to initialize LIS3DH accelerometer!\n"); // Prints a failure message over USB serial
        for (uint8_t r = 0; r < MATRIX_SIZE; r++) // Loops through each row of the LED matrix
            for (uint8_t c = 0; c < MATRIX_SIZE; c++) // Loops through each column of the LED matrix
                matrix.set_pixel(r, c, RED); // Fills every pixel on the matrix with the RED color to indicate error state
        matrix.write(); // Transmits the pixel buffer data out to update the physical LEDs
        sleep_ms(1000); // Waits for 1 second before retrying accelerometer initialization
    }

    while (true) { // Enters the main infinite application processing loop
        accel.update(); // Updates acceleration data by reading the latest values from the LIS3DH sensor
        printf("x: %6.3f g  y: %6.3f g  z: %6.3f g\n", accel.x, accel.y, accel.z); // Prints current X, Y, and Z acceleration values to serial
        matrix.clear_pixels(); // Clears all pixels in the matrix buffer (turns them off)

        bool level = (accel.x > -LEVEL_THRESHOLD && accel.x < LEVEL_THRESHOLD && // Evaluates if X acceleration is within the level threshold
                      accel.y > -LEVEL_THRESHOLD && accel.y < LEVEL_THRESHOLD); // Evaluates if Y acceleration is also within the level threshold

        if (level) { // Checks if the device is currently level on both X and Y axes
            // Center 4 LEDs
            matrix.set_pixel(3, 3, GREEN); // Illuminates the top-left center pixel green
            matrix.set_pixel(3, 4, GREEN); // Illuminates the top-right center pixel green
            matrix.set_pixel(4, 3, GREEN); // Illuminates the bottom-left center pixel green
            matrix.set_pixel(4, 4, GREEN); // Illuminates the bottom-right center pixel green
        } else { // Runs if the device is tilted (not level)
            // x tilt moves the dot left/right, y tilt moves it up/down.
            // If the dot moves opposite to what you expect, negate x or y here.
            uint8_t col = accel_to_index(accel.x); // Maps X acceleration to a matrix column index
            uint8_t row = accel_to_index(accel.y); // Maps Y acceleration to a matrix row index
            matrix.set_pixel(row, col, RED); // Illuminates the corresponding pixel red to indicate tilt direction/position
        }

        matrix.write(); // Sends the updated pixel buffer data to the physical LED hardware
        sleep_ms(50); // Pauses for 50 milliseconds to maintain a stable refresh rate (~20 Hz)
    }
}