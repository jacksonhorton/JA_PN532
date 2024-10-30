#include "PN532_SPI.h"
#include "PN532_debug.h"
#include "Particle.h"

#define STATUS_READ     2
#define DATA_WRITE      1
#define DATA_READ       3

PN532_SPI::PN532_SPI(SPIClass &spi, uint8_t ss) 
    : _spi(&spi), _ss(ss), command(0) {}

void PN532_SPI::begin() {
    pinMode(_ss, OUTPUT);

    _spi->begin();
    _spi->setDataMode(SPI_MODE0);        // PN532 supports only mode0
    _spi->setBitOrder(LSBFIRST);
    _spi->setClockSpeed(2000000);        // 2MHz (max 5MHz for PN532)
}

void PN532_SPI::wakeup() {
    digitalWrite(_ss, LOW);
    delay(2);
    digitalWrite(_ss, HIGH);
}

int8_t PN532_SPI::writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen) {
    command = header[0];
    writeFrame(header, hlen, body, blen);
    
    
    uint8_t timeout = PN532_ACK_WAIT_TIME;
    while (!isReady()) {
      delay(1);
      timeout--;
      if (timeout == 0) {
	Log.info("Time out when waiting for ACK");
	return -2;
      }
    }
    if (readAckFrame()) {
      Log.info("Invalid ACK");
      return PN532_INVALID_ACK;
    }
    return 0;
}

int16_t PN532_SPI::readResponse(uint8_t buf[], uint8_t len, uint16_t timeout) {
  uint16_t time = 0;
  while (!isReady()) {
    delay(1);
    if (timeout > 0 && ++time > timeout) return PN532_TIMEOUT;
  }

  digitalWrite(_ss, LOW);
  delay(1);
  write(DATA_READ);

  int16_t result = PN532_INVALID_FRAME;

  if (read() == 0x00 && read() == 0x00 && read() == 0xFF) { // Check Preamble and Start Codes
    uint8_t length = read();
    if ((length + read()) == 0) {  // Length checksum
      if (read() == PN532_PN532TOHOST && read() == command + 1) {
	Log.info("read:  0x%02X", command + 1);

	uint8_t sum = PN532_PN532TOHOST + command + 1;
	for (uint8_t i = 0; i < length - 2 && i < len; i++) {
	  buf[i] = read();
	  sum += buf[i];
	}

	if ((uint8_t)(sum + read()) == 0) {  // Data checksum
	  read(); // POSTAMBLE
	  result = length - 2;
	} else {
	  Log.info("Checksum is not OK");
	}
      }
    }
  }

  digitalWrite(_ss, HIGH);
  return result;
}

bool PN532_SPI::isReady() {
  digitalWrite(_ss, LOW);
  write(STATUS_READ);
  bool ready = (read() & 1);
  digitalWrite(_ss, HIGH);
  return ready;
}

void PN532_SPI::writeFrame(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen) {
  digitalWrite(_ss, LOW);
  delay(2);  // Wake up PN532

  write(DATA_WRITE);
  write(PN532_PREAMBLE);
  write(PN532_STARTCODE1);
  write(PN532_STARTCODE2);

  uint8_t length = hlen + blen + 1;
  write(length);
  write(~length + 1);

  write(PN532_HOSTTOPN532);
  uint8_t sum = PN532_HOSTTOPN532;

  for (uint8_t i = 0; i < hlen; i++) {
    write(header[i]);
    sum += header[i];
  }
  for (uint8_t i = 0; i < blen; i++) {
    write(body[i]);
    sum += body[i];
  }

  write(~sum + 1);
  write(PN532_POSTAMBLE);

  digitalWrite(_ss, HIGH);
}

int8_t PN532_SPI::readAckFrame() {
  const uint8_t PN532_ACK[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
  uint8_t ackBuf[sizeof(PN532_ACK)];

  digitalWrite(_ss, LOW);
  delay(1);
  write(DATA_READ);

  for (uint8_t i = 0; i < sizeof(PN532_ACK); i++) {
    ackBuf[i] = read();
  }

  digitalWrite(_ss, HIGH);
  return memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK)) == 0 ? 0 : -1;
}~#include "PN532_SPI.h"
#include "PN532_debug.h"
#include "Particle.h"

#define STATUS_READ     2
#define DATA_WRITE      1
#define DATA_READ       3

PN532_SPI::PN532_SPI(SPIClass &spi, uint8_t ss) 
    : _spi(&spi), _ss(ss), command(0) {}

void PN532_SPI::begin() {
    pinMode(_ss, OUTPUT);

    _spi->begin();
    _spi->setDataMode(SPI_MODE0);        // PN532 supports only mode0
    _spi->setBitOrder(LSBFIRST);
    _spi->setClockSpeed(2000000);        // 2MHz (max 5MHz for PN532)
}

void PN532_SPI::wakeup() {
    digitalWrite(_ss, LOW);
    delay(2);
    digitalWrite(_ss, HIGH);
}

int8_t PN532_SPI::writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen) {
    command = header[0];
    writeFrame(header, hlen, body, blen);
    
    uint8_t timeout = PN532_ACK_WAIT_TIME;
    while (!isReady()) {
        delay(1);
        timeout--;
        if (timeout == 0) {
            Log.info("Time out when waiting for ACK");
            return -2;
        }
    }
    if (readAckFrame()) {
        Log.info("Invalid ACK");
        return PN532_INVALID_ACK;
    }
    return 0;
}

int16_t PN532_SPI::readResponse(uint8_t buf[], uint8_t len, uint16_t timeout) {
    uint16_t time = 0;
    while (!isReady()) {
        delay(1);
        if (timeout > 0 && ++time > timeout) return PN532_TIMEOUT;
    }

    digitalWrite(_ss, LOW);
    delay(1);
    write(DATA_READ);

    int16_t result = PN532_INVALID_FRAME;

    if (read() == 0x00 && read() == 0x00 && read() == 0xFF) { // Check Preamble and Start Codes
        uint8_t length = read();
        if ((length + read()) == 0) {  // Length checksum
            if (read() == PN532_PN532TOHOST && read() == command + 1) {
                Log.info("read:  0x%02X", command + 1);

                uint8_t sum = PN532_PN532TOHOST + command + 1;
                for (uint8_t i = 0; i < length - 2 && i < len; i++) {
                    buf[i] = read();
                    sum += buf[i];
                }

                if ((uint8_t)(sum + read()) == 0) {  // Data checksum
                    read(); // POSTAMBLE
                    result = length - 2;
                } else {
                    Log.info("Checksum is not OK");
                }
            }
        }
    }

    digitalWrite(_ss, HIGH);
    return result;
}

bool PN532_SPI::isReady() {
    digitalWrite(_ss, LOW);
    write(STATUS_READ);
    bool ready = (read() & 1);
    digitalWrite(_ss, HIGH);
    return ready;
}

void PN532_SPI::writeFrame(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen) {
    digitalWrite(_ss, LOW);
    delay(2);  // Wake up PN532

    write(DATA_WRITE);
    write(PN532_PREAMBLE);
    write(PN532_STARTCODE1);
    write(PN532_STARTCODE2);

    uint8_t length = hlen + blen + 1;
    write(length);
    write(~length + 1);

    write(PN532_HOSTTOPN532);
    uint8_t sum = PN532_HOSTTOPN532;

    for (uint8_t i = 0; i < hlen; i++) {
        write(header[i]);
        sum += header[i];
    }
    for (uint8_t i = 0; i < blen; i++) {
        write(body[i]);
        sum += body[i];
    }

    write(~sum + 1);
    write(PN532_POSTAMBLE);

    digitalWrite(_ss, HIGH);
}

int8_t PN532_SPI::readAckFrame() {
    const uint8_t PN532_ACK[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
    uint8_t ackBuf[sizeof(PN532_ACK)];

    digitalWrite(_ss, LOW);
    delay(1);
    write(DATA_READ);

    for (uint8_t i = 0; i < sizeof(PN532_ACK); i++) {
        ackBuf[i] = read();
    }

    digitalWrite(_ss, HIGH);
    return memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK)) == 0 ? 0 : -1;
}
