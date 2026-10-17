#ifndef LIS3DH_H
#define LIS3DH_H

#include <stdint.h>

// Driver for the LIS3DH accelerometer on the Propmaker FeatherWing.
// Talks over I2C1 (SDA = GPIO2, SCL = GPIO3 on the Feather RP2040).
class LIS3DH {
public:
    // Latest acceleration readings, in g. Refreshed by update().
    float x, y, z;

    LIS3DH(void);

    // Sets up the I2C bus and the sensor. Returns false if the sensor
    // does not answer with the expected WHO_AM_I value.
    bool init(void);

    // Write one byte to a sensor register.
    void set_reg(uint8_t reg, uint8_t val);

    // Read one byte from a sensor register.
    uint8_t read_reg(uint8_t reg);

    // Read the three axes and store them in x, y, z.
    void update(void);

private:
    // Read a 16-bit axis value (low byte at reg, high byte at reg + 1)
    // and convert it to g.
    float read_axis(uint8_t reg_low);
};

#endif
