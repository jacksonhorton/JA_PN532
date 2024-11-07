// This #include statement was automatically added by the Particle IDE.
#include <ParticlePN532.h>
#include "Particle.h"
#include "PN532_I2C.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Initialize I2C interface and NFC adapter
PN532_I2C pn532_i2c(Wire);
ParticlePN532 nfc(pn532_i2c);

// set log level to LOG_LEVEL_TRACE or LOG_LEVEL_ALL to see debug messages.
// set to LOG_LEVEL_INFO to see general stuff
SerialLogHandler logHandler(LOG_LEVEL_INFO);

void setup()
{
    Log.info("PN532 Reader initializing...");
    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    while (!versiondata)
    {
        Serial.print("Didn't find PN53x board");
        delay(1000);
        versiondata = nfc.getFirmwareVersion();
    }
    // Got ok data, print it out!
    Log.info("Found chip PN5%X", (versiondata >> 24) & 0xFF);
    Log.info("Firmware ver. %d.%d", (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);

    // configure board to read RFID tags
    nfc.SAMConfig();

    Log.info("Waiting for an ISO14443A Card ...");
}

void loop()
{
    uint8_t success;
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
    uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success)
    {
        // Display some basic information about the card
        Log.info("Found an ISO14443A card");
        Log.info("UID Length: %d bytes", uidLength);
        Log.info("UID Value:");
        nfc.PrintHex(uid, uidLength);

        if (uidLength == 4)
        {
            // We probably have a Mifare Classic card ...
            Log.info("Seems to be a Mifare Classic card (4 byte UID)");

            // Now we need to try to authenticate it for read/write access
            // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
            Log.info("Trying to authenticate block 4 with default KEYA value");
            uint8_t keya[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

            // Start with block 4 (the first block of sector 1) since sector 0
            // contains the manufacturer data and it's probably better just
            // to leave it alone unless you know what you're doing
            success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

            if (success)
            {
                Log.info("Sector 1 (Blocks 4..7) has been authenticated");
                uint8_t data[16];

                // If you want to write something to block 4 to test with, uncomment
                // the following line and this text should be read back in a minute
                // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
                // success = nfc.mifareclassic_WriteDataBlock (4, data);

                // Try to read the contents of block 4
                success = nfc.mifareclassic_ReadDataBlock(4, data);

                if (success)
                {
                    // Data seems to have been read ... spit it out
                    Log.info("Reading Block 4:");
                    nfc.PrintHexChar(data, 16);

                    // Wait a bit before reading the card again
                    delay(1000);
                }
                else
                {
                    Log.info("Ooops ... unable to read the requested block.  Try another key?");
                }
            }
            else
            {
                Log.info("Ooops ... authentication failed: Try another key?");
            }
        }

        if (uidLength == 7)
        {
            // We probably have a Mifare Ultralight card ...
            Log.info("Seems to be a Mifare Ultralight tag (7 byte UID)");

            // Try to read the first general-purpose user page (#4)
            Log.info("Reading page 4");
            uint8_t data[32];
            success = nfc.mifareultralight_ReadPage(4, data);
            if (success)
            {
                // Data seems to have been read ... spit it out
                nfc.PrintHexChar(data, 4);

                // Wait a bit before reading the card again
                delay(1000);
            }
            else
            {
                Log.info("Ooops ... unable to read the requested page!?");
            }
        }
    }
}
