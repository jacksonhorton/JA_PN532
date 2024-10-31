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
    Log.trace("NDEF Reader initializing...");
    nfc.begin(true);
}

void loop() {
    Log.info("Scan a NFC tag");
    if (nfc.tagPresent()) {
        NfcTag tag = nfc.read();
        tag.print();
        
        if (tag.hasNdefMessage()) // every tag won't have a message
        {
    
            NdefMessage message = tag.getNdefMessage();
            Serial.print("\nThis NFC Tag contains an NDEF Message with ");
            Serial.print(message.getRecordCount());
            Serial.print(" NDEF Record");
            if (message.getRecordCount() != 1) {
            Serial.print("s");
            }
            Serial.println(".");
            
            // cycle through the records, printing some info from each
            int recordCount = message.getRecordCount();
            for (int i = 0; i < recordCount; i++)
            {
                Log.info("\nNDEF Record %d", i+1);
                NdefRecord record = message.getRecord(i);
                // NdefRecord record = message[i]; // alternate syntax
                
                Log.info("TNF: %s", String(record.getTnf()).c_str());
                Log.info("Type: %s", String(record.getType()).c_str());
                
                // The TNF and Type should be used to determine how your application processes the payload
                // There's no generic processing for the payload, it's returned as a byte[]
                int payloadLength = record.getPayloadLength();
                uint8_t payload[payloadLength];
                record.getPayload(payload);
                
                // Print the Hex and Printable Characters
                Serial.print("  Payload (HEX): ");
                PrintHexChar(payload, payloadLength);
                
                // Force the data into a String (might work depending on the content)
                // Real code should use smarter processing
                String payloadAsString = "";
                for (int c = 0; c < payloadLength; c++) {
                    payloadAsString += (char)payload[c];
                }
                Log.info("  Payload (as String): %s", payloadAsString);
                
                // id is probably blank and will return ""
                String uid = record.getId();
                if (uid != "") {
                    Log.info("  ID: %s", uid);
                }
            }
                    
        }
        delay(5000);
    }
}
