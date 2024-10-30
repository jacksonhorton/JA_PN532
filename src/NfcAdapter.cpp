#include <NfcAdapter.h>

NfcAdapter::NfcAdapter(PN532Interface &interface)
{
  shield = new JA_PN532(interface);
}

NfcAdapter::~NfcAdapter(void)
{
  delete shield;
}

void NfcAdapter::begin(boolean verbose)
{
  shield->begin();

  uint32_t versiondata = shield->getFirmwareVersion();

  if (!versiondata)
    {
      Log.info("Didn't find PN53x board");
      while (1) delay(1000); // halt
    }

  if (verbose)
    {
      Log.info("Found chip PN5%02X", (versiondata >> 24) & 0xFF);
      Log.info("Firmware ver. %d.%d", (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);
    }
  // configure board to read RFID tags
  shield->SAMConfig();
}

boolean NfcAdapter::tagPresent(unsigned long timeout)
{
  uint8_t success;
  uidLength = 0;

  if (timeout == 0)
    {
      success = shield->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, (uint8_t*)&uidLength);
    }
  else
    {
      success = shield->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, (uint8_t*)&uidLength, timeout);
    }
  return success;
}

boolean NfcAdapter::erase()
{
  boolean success;
  NdefMessage message = NdefMessage();
  message.addEmptyRecord();
  return write(message);
}

boolean NfcAdapter::format()
{
  boolean success;
  if (uidLength == 4)
    {
      MifareClassic mifareClassic = MifareClassic(*shield);
      success = mifareClassic.formatNDEF(uid, uidLength);
    }
  else
    {
      Log.info("Unsupported Tag.");
      success = false;
    }
  return success;
}

boolean NfcAdapter::clean()
{
  uint8_t type = guessTagType();

  if (type == TAG_TYPE_MIFARE_CLASSIC)
    {
#ifdef NDEF_DEBUG
      Log.info("Cleaning Mifare Classic");
#endif
      MifareClassic mifareClassic = MifareClassic(*shield);
      return mifareClassic.formatMifare(uid, uidLength);
    }
  else if (type == TAG_TYPE_2)
    {
#ifdef NDEF_DEBUG
      Log.info("Cleaning Mifare Ultralight");
#endif
      MifareUltralight ultralight = MifareUltralight(*shield);
      return ultralight.clean();
    }
  else
    {
      Log.info("No driver for card type %d", type);
      return false;
    }

}


NfcTag NfcAdapter::read()
{
  uint8_t type = guessTagType();

  if (type == TAG_TYPE_MIFARE_CLASSIC)
    {
#ifdef NDEF_DEBUG
      Log.info("Reading Mifare Classic");
#endif
      MifareClassic mifareClassic = MifareClassic(*shield);
      return mifareClassic.read(uid, uidLength);
    }
  else if (type == TAG_TYPE_2)
    {
#ifdef NDEF_DEBUG
      Log.info("Reading Mifare Ultralight");
#endif
      MifareUltralight ultralight = MifareUltralight(*shield);
      return ultralight.read(uid, uidLength);
    }
  else if (type == TAG_TYPE_UNKNOWN)
    {
      Log.info("Can not determine tag type");
      return NfcTag(uid, uidLength);
    }
  else
    {
      Log.info("No driver for card type %d", type);
      // TODO should set type here
      return NfcTag(uid, uidLength);
    }

}

boolean NfcAdapter::write(NdefMessage& ndefMessage)
{
  boolean success;
  uint8_t type = guessTagType();

  if (type == TAG_TYPE_MIFARE_CLASSIC)
    {
#ifdef NDEF_DEBUG
      Log.info("Writing Mifare Classic");
#endif
      MifareClassic mifareClassic = MifareClassic(*shield);
      success = mifareClassic.write(ndefMessage, uid, uidLength);
    }
  else if (type == TAG_TYPE_2)
    {
#ifdef NDEF_DEBUG
      Log.info("Writing Mifare Ultralight");
#endif
      MifareUltralight mifareUltralight = MifareUltralight(*shield);
      success = mifareUltralight.write(ndefMessage, uid, uidLength);
    }
  else if (type == TAG_TYPE_UNKNOWN)
    {
      Log.info("Can not determine tag type");
      success = false;
    }
  else
    {
      Log.info("No driver for card type %d", type);
      success = false;
    }

  return success;
}

// TODO this should return a Driver MifareClassic, MifareUltralight, Type 4, Unknown
// Guess Tag Type by looking at the ATQA and SAK values
// Need to follow spec for Card Identification. Maybe AN1303, AN1305 and ???
unsigned int NfcAdapter::guessTagType()
{

  // 4 byte id - Mifare Classic
  //  - ATQA 0x4 && SAK 0x8
  // 7 byte id
  //  - ATQA 0x44 && SAK 0x8 - Mifare Classic
  //  - ATQA 0x44 && SAK 0x0 - Mifare Ultralight NFC Forum Type 2
  //  - ATQA 0x344 && SAK 0x20 - NFC Forum Type 4

  if (uidLength == 4)
    {
      return TAG_TYPE_MIFARE_CLASSIC;
    }
  else
    {
      return TAG_TYPE_2;
    }
}
