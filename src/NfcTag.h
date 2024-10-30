
#ifndef NfcTag_h
#define NfcTag_h

#include <Particle.h>
#include <NdefMessage.h>

class NfcTag
{
 public:
  NfcTag();
  NfcTag(uint8_t *uid, unsigned int uidLength);
  NfcTag(uint8_t *uid, unsigned int uidLength, String tagType);
  NfcTag(uint8_t *uid, unsigned int uidLength, String tagType, NdefMessage& ndefMessage);
  NfcTag(uint8_t *uid, unsigned int uidLength, String tagType, const uint8_t *ndefData, const int ndefDataLength);
  ~NfcTag();
  NfcTag& operator=(const NfcTag& rhs);
  uint8_t getUidLength();
  void getUid(uint8_t *uid, unsigned int uidLength);
  String getUidString();
  String getTagType();
  bool hasNdefMessage();
  NdefMessage getNdefMessage();
  void print();

 private:
  uint8_t *_uid;
  unsigned int _uidLength;
  String _tagType; // Mifare Classic, NFC Forum Type {1,2,3,4}, Unknown
  NdefMessage* _ndefMessage;
  // TODO: capacity, isFormatted
};

#endif
