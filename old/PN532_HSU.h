#ifndef __PN532_HSU_H__
#define __PN532_HSU_H__

#include "PN532Interface.h"
#include "Serial2/Serial2.h"
#include "Particle.h"

// Uncomment if you need to debug
//#define PN532_HSU_DEBUG

#define PN532_HSU_READ_TIMEOUT (1000)

class PN532_HSU : public PN532Interface {
public:
    // Constructor using Serial1 directly (Photon 2 uses USARTSerial)
    PN532_HSU();

    void begin();
    void wakeup();
    virtual int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body = nullptr, uint8_t blen = 0);
    int16_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout);

private:
    USARTSerial* _serial;
    uint8_t command;

    int8_t readAckFrame();
    int8_t receive(uint8_t *buf, int len, uint16_t timeout = PN532_HSU_READ_TIMEOUT);
};

#endif
