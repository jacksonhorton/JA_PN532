#include <NdefMessage.h>
#include <cstdint>

NdefMessage::NdefMessage(void)
{
  _recordCount = 0;
}

NdefMessage::NdefMessage(const uint8_t * data, const int numBytes)
{
#ifdef NDEF_DEBUG
  Log.trace("Decoding %d bytes", numBytes);
  PrintHexChar(data, numBytes);
  // DumpHex(data, numBytes, 16);
#endif

  _recordCount = 0;
  int index = 0;

  while (index <= numBytes)
    {
      uint8_t tnf_byte = data[index];
      bool mb = (tnf_byte & 0x80) != 0;
      bool me = (tnf_byte & 0x40) != 0;
      bool cf = (tnf_byte & 0x20) != 0;
      bool sr = (tnf_byte & 0x10) != 0;
      bool il = (tnf_byte & 0x8) != 0;
      uint8_t tnf = (tnf_byte & 0x7);

      NdefRecord record = NdefRecord();
      record.setTnf(tnf);

      index++;
      int typeLength = data[index];

      int payloadLength = 0;
      if (sr)
        {
	  index++;
	  payloadLength = data[index];
        }
      else
        {
	  index++;
	  uint32_t byte1 = (0xFF & data[index]) << 24;
	  index++;
	  uint32_t byte2 = (0xFF & data[index]) << 16;
	  index++;
	  uint32_t byte3 = (0xFF & data[index]) << 8;
	  index++;
	  uint32_t byte4 = 0xFF & data[index];

	  payloadLength = byte1 | byte2 | byte3 | byte4;

        }

      int idLength = 0;
      if (il)
        {
	  index++;
	  idLength = data[index];
        }

      index++;
      record.setType(&data[index], typeLength);
      index += typeLength;

      if (il)
        {
	  record.setId(&data[index], idLength);
	  index += idLength;
        }

      record.setPayload(&data[index], payloadLength);
      index += payloadLength;

      addRecord(record);

        if (me) break; // last message
    }
}

NdefMessage::NdefMessage(const NdefMessage& rhs)
{
    _recordCount = rhs._recordCount;
    for (int i = 0; i < _recordCount; i++)
    {
        _records[i] = rhs._records[i];
    }
}

NdefMessage::~NdefMessage()
{
}

NdefMessage& NdefMessage::operator=(const NdefMessage& rhs)
{
    if (this != &rhs)
    {
        for (int i = 0; i < _recordCount; i++)
        {
            _records[i] = NdefRecord();
        }

        _recordCount = rhs._recordCount;
        for (int i = 0; i < _recordCount; i++)
        {
            _records[i] = rhs._records[i];
        }
    }
    return *this;
}

unsigned int NdefMessage::getRecordCount()
{
    return _recordCount;
}

int NdefMessage::getEncodedSize()
{
    int size = 0;
    for (int i = 0; i < _recordCount; i++)
    {
        size += _records[i].getEncodedSize();
    }
    return size;
}

void NdefMessage::encode(uint8_t* data)
{
    uint8_t* data_ptr = &data[0];

    for (int i = 0; i < _recordCount; i++)
    {
        _records[i].encode(data_ptr, i == 0, (i + 1) == _recordCount);
        data_ptr += _records[i].getEncodedSize();
    }
}

bool NdefMessage::addRecord(NdefRecord& record)
{
    if (_recordCount < MAX_NDEF_RECORDS)
    {
        _records[_recordCount] = record;
        _recordCount++;
        return true;
    }
    else
    {
        Log.warn("Too many records. Increase MAX_NDEF_RECORDS.");
        return false;
    }
}

void NdefMessage::addMimeMediaRecord(String mimeType, String payload)
{
    uint8_t payloadBytes[payload.length() + 1];
    payload.getBytes(payloadBytes, sizeof(payloadBytes));

    addMimeMediaRecord(mimeType, payloadBytes, payload.length());
}

void NdefMessage::addMimeMediaRecord(String mimeType, uint8_t* payload, int payloadLength)
{
    NdefRecord r = NdefRecord();
    r.setTnf(TNF_MIME_MEDIA);

    uint8_t type[mimeType.length() + 1];
    mimeType.getBytes(type, sizeof(type));
    r.setType(type, mimeType.length());

    r.setPayload(payload, payloadLength);

    addRecord(r);
}

void NdefMessage::addTextRecord(String text)
{
    addTextRecord(text, "en");
}

void NdefMessage::addTextRecord(String text, String encoding)
{
    NdefRecord r = NdefRecord();
    r.setTnf(TNF_WELL_KNOWN);

    uint8_t RTD_TEXT[1] = { 0x54 };
    r.setType(RTD_TEXT, sizeof(RTD_TEXT));

    String payloadString = "X" + encoding + text;
    uint8_t payload[payloadString.length() + 1];
    payloadString.getBytes(payload, sizeof(payload));

    payload[0] = encoding.length();
    r.setPayload(payload, payloadString.length());

    addRecord(r);
}

void NdefMessage::addUriRecord(String uri)
{
    NdefRecord* r = new NdefRecord();
    r->setTnf(TNF_WELL_KNOWN);

    uint8_t RTD_URI[1] = { 0x55 };
    r->setType(RTD_URI, sizeof(RTD_URI));

    String payloadString = "X" + uri;
    uint8_t payload[payloadString.length() + 1];
    payloadString.getBytes(payload, sizeof(payload));

    payload[0] = 0x0;
    r->setPayload(payload, payloadString.length());

    addRecord(*r);
    delete(r);
}

void NdefMessage::addEmptyRecord()
{
    NdefRecord* r = new NdefRecord();
    r->setTnf(TNF_EMPTY);
    addRecord(*r);
    delete(r);
}

NdefRecord NdefMessage::getRecord(int index)
{
    if (index > -1 && index < _recordCount)
    {
        return _records[index];
    }
    else
    {
        return NdefRecord();
    }
}

NdefRecord NdefMessage::operator[](int index)
{
    return getRecord(index);
}

void NdefMessage::print()
{
    Log.trace("\nNDEF Message %d record%s, %d bytes", _recordCount, _recordCount == 1 ? "" : "s", getEncodedSize());

    for (int i = 0; i < _recordCount; i++)
    {
         _records[i].print();
    }
}
