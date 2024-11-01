#ifndef MifareClassic_h
#define MifareClassic_h

#include <Particle.h>
#include <JA_PN532.h>
#include <Ndef.h>
#include <NfcTag.h>

class MifareClassic
{
 public:
  MifareClassic(JA_PN532& nfcShield);
  ~MifareClassic();
  NfcTag read(uint8_t *uid, unsigned int uidLength);
  bool write(NdefMessage& ndefMessage, uint8_t *uid, unsigned int uidLength);
  bool formatNDEF(uint8_t *uid, unsigned int uidLength);
  bool formatMifare(uint8_t *uid, unsigned int uidLength);

 private:
  JA_PN532* _nfcShield;
  int getBufferSize(int messageLength);
  int getNdefStartIndex(uint8_t *data);
  bool decodeTlv(uint8_t *data, int &messageLength, int &messageStartIndex);
};

#endif
