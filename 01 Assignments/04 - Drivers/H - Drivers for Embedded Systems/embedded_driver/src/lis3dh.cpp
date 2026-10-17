#include "lis3dh.h" // Includes the LIS3DH driver class declaration header
#include "hardware/i2c.h" // Includes the Raspberry Pi Pico hardware I2C API header
#include "hardware/gpio.h" // Includes the Raspberry Pi Pico hardware GPIO configuration header
#include "pico/binary_info.h" // Includes binary information support for the Pico SDK
#include "pico/stdlib.h" // Includes core Pico standard library functions and types

// Hardware connections (Feather RP2040 + Propmaker FeatherWing)
#define LIS3DH_I2C      i2c1    // GPIO2/3 can only be used with I2C1 // Defines the I2C peripheral instance used (i2c1)
#define LIS3DH_SDA_PIN  2 // Defines the GPIO pin number (GPIO 2) used for I2C data (SDA)
#define LIS3DH_SCL_PIN  3 // Defines the GPIO pin number (GPIO 3) used for I2C clock (SCL)
#define LIS3DH_I2C_ADDR 0x18 // Defines the 7-bit I2C device slave address for the LIS3DH accelerometer

#define LIS3DH_REG_WHO_AM_I 0x0F // Defines the register address for the device identification WHO_AM_I register
#define LIS3DH_REG_CTRL1 0x20 // Defines the control register 1 address for setting data rate and power modes
#define LIS3DH_REG_CTRL4 0x23 // Defines the control register 4 address for setting resolution and full-scale range
#define LIS3DH_REG_OUT_X_L 0x28 // Defines the starting data output register address for the X-axis low byte
#define LIS3DH_REG_OUT_Y_L     0x2A // Defines the starting data output register address for the Y-axis low byte
#define LIS3DH_REG_OUT_Z_L     0x2C // Defines the starting data output register address for the Z-axis low byte

#define WHO_AM_I_VALUE  0x33 // Defines the expected chip identification byte value (0x33 for LIS3DH)

LIS3DH::LIS3DH(void) { // Constructor for the LIS3DH class
    x = 0.0f; // Initializes member variable x acceleration to 0.0
    y = 0.0f; // Initializes member variable y acceleration to 0.0
    z = 0.0f; // Initializes member variable z acceleration to 0.0
    addr = LIS3DH_I2C_ADDR; // Assigns the defined I2C address to the class instance variable
}

bool LIS3DH::init(void) { // Initialization method for setting up I2C communication and configuring the sensor
    // Initialize I2C1 at 400kHz. Pins: SDA = GPIO2, SCL = GPIO3 on Feather RP2040
    i2c_init(LIS3DH_I2C, 400 * 1000); // Initializes the I2C peripheral at a 400 kHz bus clock frequency
    gpio_set_function(LIS3DH_SDA_PIN, GPIO_FUNC_I2C); // Configures SDA pin (GPIO 2) to function as an I2C peripheral pin
    gpio_set_function(LIS3DH_SCL_PIN, GPIO_FUNC_I2C); // Configures SCL pin (GPIO 3) to function as an I2C peripheral pin
    gpio_pull_up(LIS3DH_SDA_PIN); // Enables the internal pull-up resistor on the SDA pin
    gpio_pull_up(LIS3DH_SCL_PIN); // Enables the internal pull-up resistor on the SCL pin

    // Verify accelerometer WHO_AM_I register (expects 0x33)
    if (read_reg(LIS3DH_REG_WHO_AM_I) != WHO_AM_I_VALUE) { // Reads WHO_AM_I register and checks if it matches the expected 0x33 value
        return false; // Returns false if the device ID does not match, indicating initialization failure
    }

    // Enable X, Y, Z axes, normal power mode, 1.34 kHz data, rate
    set_reg(LIS3DH_REG_CTRL1, 0x97); // Configures control register 1 with 0x97 to enable all axes and set output data rate

    // High resolution, 4g range -> CTRL4 = 0x08
    // Blocck date update on, +/-2 g full scale
    set_reg(LIS3DH_REG_CTRL4, 0x08); // Configures control register 4 with 0x08 for scale and block data update settings

    return true; // Returns true indicating successful hardware initialization and configuration
}

void LIS3DH::set_reg(uint8_t reg, uint8_t val) { // Helper method to write a single byte value to a specified register
    uint8_t buf[2] = {reg, val}; // Creates a 2-byte buffer containing the register address and the value to write
    i2c_write_blocking(LIS3DH_I2C, LIS3DH_I2C_ADDR, buf, 2, false); // Performs a blocking I2C write transaction to send the buffer to the device
}

uint8_t LIS3DH::read_reg(uint8_t reg) { // Helper method to read a single byte from a specified register address
    uint8_t val = 0; // Initializes a local variable to store the received register byte
    i2c_write_blocking(LIS3DH_I2C, LIS3DH_I2C_ADDR, &reg, 1, true); // Writes the target register address with a repeated start condition (no stop)
    i2c_read_blocking(LIS3DH_I2C, LIS3DH_I2C_ADDR, &val, 1, false); // Reads 1 byte of data back from the device into val
    return val; // Returns the read register byte value
}

float LIS3DH::read_axis(uint8_t reg_low) { // Helper method to read and convert raw 16-bit data for a single acceleration axis
    uint8_t lsb = read_reg(reg_low); // Reads the low-byte register for the axis
    uint8_t msb = read_reg(reg_low + 1); // Reads the high-byte register for the axis (next consecutive address)
    int16_t raw = (int16_t)((msb << 8) | lsb); // Combines the low and high bytes into a single 16-bit signed integer

    // Data is left-justified 10-bit in normal mode: shift down by 6
    // (divide by 64), then 4 mg per count at +/-2 g full scale.
    return (raw / 64.0f) * 0.004f; // Scales the raw integer value into acceleration in g's
}

void LIS3DH::update(void) { // Method to update all three axis acceleration values from the sensor
    x = read_axis(LIS3DH_REG_OUT_X_L); // Updates the x member variable with the latest X-axis reading
    y = read_axis(LIS3DH_REG_OUT_Y_L); // Updates the y member variable with the latest Y-axis reading
    z = read_axis(LIS3DH_REG_OUT_Z_L); // Updates the z member variable with the latest Z-axis reading
}