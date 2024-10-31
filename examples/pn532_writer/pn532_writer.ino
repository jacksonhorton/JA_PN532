// This #include statement was automatically added by the Particle IDE.
#include <JA_PN532.h>

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

#include "Particle.h"
#include "PN532_I2C.h"
#include "NfcAdapter.h"

// Initialize I2C interface and NFC adapter
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

// set log level to LOG_LEVEL_TRACE or LOG_LEVEL_ALL to see debug messages.
// set to LOG_LEVEL_INFO to see general stuff
SerialLogHandler logHandler(LOG_LEVEL_INFO);

void setup() {
    Log.trace("NDEF Writer initializing...");
    nfc.begin(true);
}

void loop() {
    Log.info("Scan a NFC tag");
    if (nfc.tagPresent()) {
        NdefMessage message = NdefMessage();
        message.addUriRecord("http://arduino.cc");

        bool success = nfc.write(message);
        if (success) {
          Serial.println("Success. Try reading this tag with your phone.");        
        } else {
          Serial.println("Write failed.");
        }
    }
    delay(5000);
}