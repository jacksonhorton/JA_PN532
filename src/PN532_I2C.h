/**
 * @modified picospuch for Particle
 */

#ifndef __PN532_I2C_H__
#define __PN532_I2C_H__

<<<<<<< HEAD
#include "Particle.h"  // Include Particle header for Particle-specific definitions
=======
#include "Particle.h"
>>>>>>> da2af44 (Already added NDEF stuff and might have broken I2c along the way. Trying to fix it now.)
#include "PN532Interface.h"

class PN532_I2C : public PN532Interface {
public:
    PN532_I2C(TwoWire &wire);

    void begin();
    void wakeup();
    virtual int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body = 0, uint8_t blen = 0);
    int16_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout);

private:
    TwoWire* _wire;
    uint8_t command;

    int8_t readAckFrame();
    int16_t getResponseLength(uint8_t buf[], uint8_t len, uint16_t timeout);

    inline uint8_t write(uint8_t data) {
        return _wire->write(data); // Use write directly on Particle
    }

    inline uint8_t read() {
        return _wire->read(); // Use read directly on Particle
    }
};

#endif
