#include <MifareUltralight.h>

#define ULTRALIGHT_PAGE_SIZE 4
#define ULTRALIGHT_READ_SIZE 4 // we should be able to read 16 bytes at a time

#define ULTRALIGHT_DATA_START_PAGE 4
#define ULTRALIGHT_MESSAGE_LENGTH_INDEX 1
#define ULTRALIGHT_DATA_START_INDEX 2
#define ULTRALIGHT_MAX_PAGE 63

#define NFC_FORUM_TAG_TYPE_2 ("NFC Forum Type 2")

MifareUltralight::MifareUltralight(JA_PN532& nfcShield)
{
    nfc = &nfcShield;
    ndefStartIndex = 0;
    messageLength = 0;
}

MifareUltralight::~MifareUltralight()
{
}

NfcTag MifareUltralight::read(uint8_t * uid, unsigned int uidLength)
{
    if (isUnformatted())
    {
        Log.info("WARNING: Tag is not formatted.");
        return NfcTag(uid, uidLength, NFC_FORUM_TAG_TYPE_2);
    }

    readCapabilityContainer(); // meta info for tag
    findNdefMessage();
    calculateBufferSize();

    if (messageLength == 0) { // data is 0x44 0x03 0x00 0xFE
        NdefMessage message = NdefMessage();
        message.addEmptyRecord();
        return NfcTag(uid, uidLength, NFC_FORUM_TAG_TYPE_2, message);
    }

    bool success;
    uint8_t page;
    uint8_t index = 0;
    uint8_t buffer[bufferSize];
    for (page = ULTRALIGHT_DATA_START_PAGE; page < ULTRALIGHT_MAX_PAGE; page++)
    {
        // read the data
        success = nfc->mifareultralight_ReadPage(page, &buffer[index]);
        if (success)
        {
            #ifdef MIFARE_ULTRALIGHT_DEBUG
            Log.info("Page %d ", page);
            nfc->PrintHexChar(&buffer[index], ULTRALIGHT_PAGE_SIZE);
            #endif
        }
        else
        {
            Log.info("Read failed %d", page);
            messageLength = 0;
            break;
        }

        if (index >= (messageLength + ndefStartIndex))
        {
            break;
        }

        index += ULTRALIGHT_PAGE_SIZE;
    }

    NdefMessage ndefMessage = NdefMessage(&buffer[ndefStartIndex], messageLength);
    return NfcTag(uid, uidLength, NFC_FORUM_TAG_TYPE_2, ndefMessage);
}

bool MifareUltralight::isUnformatted()
{
    uint8_t page = 4;
    uint8_t data[ULTRALIGHT_READ_SIZE];
    bool success = nfc->mifareultralight_ReadPage(page, data);
    if (success)
    {
        return (data[0] == 0xFF && data[1] == 0xFF && data[2] == 0xFF && data[3] == 0xFF);
    }
    else
    {
        Log.info("Error. Failed read page %d", page);
        return false;
    }
}

// page 3 has tag capabilities
void MifareUltralight::readCapabilityContainer()
{
    uint8_t data[ULTRALIGHT_PAGE_SIZE];
    int success = nfc->mifareultralight_ReadPage(3, data);
    if (success)
    {
        tagCapacity = data[2] * 8;
        #ifdef MIFARE_ULTRALIGHT_DEBUG
        Log.info("Tag capacity %d bytes", tagCapacity);
        #endif
    }
}

// read enough of the message to find the ndef message length
void MifareUltralight::findNdefMessage()
{
    int page;
    uint8_t data[12]; // 3 pages
    uint8_t* data_ptr = &data[0];

    bool success = true;
    for (page = 4; page < 6; page++)
    {
        success = success && nfc->mifareultralight_ReadPage(page, data_ptr);
        #ifdef MIFARE_ULTRALIGHT_DEBUG
        Log.info("Page %d - ", page);
        nfc->PrintHexChar(data_ptr, 4);
        #endif
        data_ptr += ULTRALIGHT_PAGE_SIZE;
    }

    if (success)
    {
        if (data[0] == 0x03)
        {
            messageLength = data[1];
            ndefStartIndex = 2;
        }
        else if (data[5] == 0x3) // page 5 byte 1
        {
            messageLength = data[6];
            ndefStartIndex = 7;
        }
    }

    #ifdef MIFARE_ULTRALIGHT_DEBUG
    Log.info("messageLength %d", messageLength);
    Log.info("ndefStartIndex %d", ndefStartIndex);
    #endif
}

// buffer is larger than the message, need to handle some data before and after
void MifareUltralight::calculateBufferSize()
{
    bufferSize = messageLength + ndefStartIndex + 1;

    if (bufferSize % ULTRALIGHT_READ_SIZE != 0)
    {
        bufferSize = ((bufferSize / ULTRALIGHT_READ_SIZE) + 1) * ULTRALIGHT_READ_SIZE;
    }
}

bool MifareUltralight::write(NdefMessage& m, uint8_t * uid, unsigned int uidLength)
{
    if (isUnformatted())
    {
        Log.info("WARNING: Tag is not formatted.");
        return false;
    }
    readCapabilityContainer();

    messageLength = m.getEncodedSize();
    ndefStartIndex = messageLength < 0xFF ? 2 : 4;
    calculateBufferSize();

    if (bufferSize > tagCapacity) {
        #ifdef MIFARE_ULTRALIGHT_DEBUG
        Log.info("Encoded Message length exceeded tag Capacity %d", tagCapacity);
        #endif
        return false;
    }

    uint8_t encoded[bufferSize];
    uint8_t *src = encoded;
    unsigned int position = 0;
    uint8_t page = ULTRALIGHT_DATA_START_PAGE;

    encoded[0] = 0x3;
    if (messageLength < 0xFF)
    {
        encoded[1] = messageLength;
    }
    else
    {
        encoded[1] = 0xFF;
        encoded[2] = ((messageLength >> 8) & 0xFF);
        encoded[3] = (messageLength & 0xFF);
    }

    m.encode(encoded + ndefStartIndex);
    memset(encoded + ndefStartIndex + messageLength, 0, bufferSize - ndefStartIndex - messageLength);
    encoded[ndefStartIndex + messageLength] = 0xFE;

    #ifdef MIFARE_ULTRALIGHT_DEBUG
    Log.info("messageLength %d", messageLength);
    Log.info("Tag Capacity %d", tagCapacity);
    nfc->PrintHex(encoded, bufferSize);
    #endif

    while (position < bufferSize)
    {
        if (!nfc->mifareultralight_WritePage(page, src))
            return false;
        #ifdef MIFARE_ULTRALIGHT_DEBUG
        Log.info("Wrote page %d - ", page);
        nfc->PrintHex(src, ULTRALIGHT_PAGE_SIZE);
        #endif
        page++;
        src += ULTRALIGHT_PAGE_SIZE;
        position += ULTRALIGHT_PAGE_SIZE;
    }
    return true;
}

// Mifare Ultralight can't be reset to factory state
bool MifareUltralight::clean()
{
    readCapabilityContainer();

    uint8_t pages = (tagCapacity / ULTRALIGHT_PAGE_SIZE) + ULTRALIGHT_DATA_START_PAGE;
    uint8_t data[4] = { 0x00, 0x00, 0x00, 0x00 };

    for (int i = ULTRALIGHT_DATA_START_PAGE; i < pages; i++) {
        #ifdef MIFARE_ULTRALIGHT_DEBUG
        Log.info("Wrote page %d - ", i);
        nfc->PrintHex(data, ULTRALIGHT_PAGE_SIZE);
        #endif
        if (!nfc->mifareultralight_WritePage(i, data)) {
            return false;
        }
    }
    return true;
}
