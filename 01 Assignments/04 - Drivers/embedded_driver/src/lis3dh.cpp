#include "lis3dh.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Hardware connections (Feather RP2040 + Propmaker FeatherWing)
#define LIS3DH_I2C      i2c1    // GPIO2/3 can only be used with I2C1
#define LIS3DH_SDA_PIN  2
#define LIS3DH_SCL_PIN  3
#define LIS3DH_ADDRESS  0x18

// Registers (from the LIS3DH datasheet)
#define REG_WHO_AM_I    0x0F
#define REG_CTRL_REG1   0x20
#define REG_CTRL_REG4   0x23
#define REG_OUT_X_L     0x28
#define REG_OUT_Y_L     0x2A
#define REG_OUT_Z_L     0x2C

#define WHO_AM_I_VALUE  0x33

LIS3DH::LIS3DH(void) : x(0.0f), y(0.0f), z(0.0f) {}

bool LIS3DH::init(void) {
    // Set up the I2C peripheral at 400 kHz and route it to the pins
    i2c_init(LIS3DH_I2C, 400 * 1000);
    gpio_set_function(LIS3DH_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(LIS3DH_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(LIS3DH_SDA_PIN);
    gpio_pull_up(LIS3DH_SCL_PIN);

    // Make sure the sensor is actually there
    if (read_reg(REG_WHO_AM_I) != WHO_AM_I_VALUE) {
        return false;
    }

    // Normal mode, 1.344 kHz data rate, X/Y/Z enabled
    set_reg(REG_CTRL_REG1, 0x97);
    // Block data update on, +/-2 g full scale
    set_reg(REG_CTRL_REG4, 0x80);

    return true;
}

void LIS3DH::set_reg(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    i2c_write_blocking(LIS3DH_I2C, LIS3DH_ADDRESS, buf, 2, false);
}

uint8_t LIS3DH::read_reg(uint8_t reg) {
    uint8_t val = 0;
    // Send the register address (keep the bus with a repeated start),
    // then read back one byte
    i2c_write_blocking(LIS3DH_I2C, LIS3DH_ADDRESS, &reg, 1, true);
    i2c_read_blocking(LIS3DH_I2C, LIS3DH_ADDRESS, &val, 1, false);
    return val;
}

float LIS3DH::read_axis(uint8_t reg_low) {
    uint8_t lsb = read_reg(reg_low);
    uint8_t msb = read_reg(reg_low + 1);
    int16_t raw = (int16_t)((msb << 8) | lsb);

    // Data is left-justified 10-bit in normal mode: shift down by 6
    // (divide by 64), then 4 mg per count at +/-2 g full scale.
    return (raw / 64.0f) * 0.004f;
}

void LIS3DH::update(void) {
    x = read_axis(REG_OUT_X_L);
    y = read_axis(REG_OUT_Y_L);
    z = read_axis(REG_OUT_Z_L);
}
