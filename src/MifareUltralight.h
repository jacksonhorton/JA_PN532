#ifndef MifareUltralight_h
#define MifareUltralight_h

#include <JA_PN532.h>
#include <NfcTag.h>
#include <Ndef.h>
#include <Particle.h>

class MifareUltralight
{
 public:
  MifareUltralight(JA_PN532& nfcShield);
  ~MifareUltralight();
  NfcTag read(uint8_t *uid, unsigned int uidLength);
  bool write(NdefMessage& ndefMessage, uint8_t *uid, unsigned int uidLength);
  bool clean();
 private:
  JA_PN532* nfc;
  unsigned int tagCapacity;
  unsigned int messageLength;
  unsigned int bufferSize;
  unsigned int ndefStartIndex;
  bool isUnformatted();
  void readCapabilityContainer();
  void findNdefMessage();
  void calculateBufferSize();
};

#endif
