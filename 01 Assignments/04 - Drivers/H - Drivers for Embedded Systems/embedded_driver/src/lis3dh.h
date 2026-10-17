#ifndef LIS3DH_H // Header guard start to prevent multiple inclusions of this header file
#define LIS3DH_H

#include <stdint.h> // Includes standard fixed-width integer types (e.g., uint8_t, int16_t)

class LIS3DH { // Declares the LIS3DH accelerometer driver class
public:
    float x; // Public member variable storing the latest X-axis acceleration in g's
    float y; // Public member variable storing the latest Y-axis acceleration in g's
    float z; // Public member variable storing the latest Z-axis acceleration in g's

    LIS3DH(void); // Constructor declaration for initializing class variables
    bool init(void); // Method declaration for initializing the I2C bus and verifying sensor presence
    void set_reg(uint8_t reg, uint8_t val); // Method declaration for writing a value to an internal sensor register
    uint8_t read_reg(uint8_t reg); // Method declaration for reading a single byte from a sensor register
    void update(void); // Method declaration for reading and refreshing all axis measurements

private:
    uint8_t addr; // Private member variable storing the device I2C slave address
    float read_axis(uint8_t reg_low); // Private helper method declaration for reading and converting individual axis data
};

#endif // LIS3DH_H // End of header guard