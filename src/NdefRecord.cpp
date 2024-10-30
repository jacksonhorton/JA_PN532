#include "NdefRecord.h"

NdefRecord::NdefRecord()
{
  _tnf = 0;
  _typeLength = 0;
  _payloadLength = 0;
  _idLength = 0;
  _type = (uint8_t *)NULL;
  _payload = (uint8_t *)NULL;
  _id = (uint8_t *)NULL;
}

NdefRecord::NdefRecord(const NdefRecord& rhs)
{
  _tnf = rhs._tnf;
  _typeLength = rhs._typeLength;
  _payloadLength = rhs._payloadLength;
  _idLength = rhs._idLength;
  _type = (uint8_t *)NULL;
  _payload = (uint8_t *)NULL;
  _id = (uint8_t *)NULL;

  if (_typeLength)
    {
      _type = (uint8_t*)malloc(_typeLength);
      memcpy(_type, rhs._type, _typeLength);
    }

  if (_payloadLength)
    {
      _payload = (uint8_t*)malloc(_payloadLength);
      memcpy(_payload, rhs._payload, _payloadLength);
    }

  if (_idLength)
    {
      _id = (uint8_t*)malloc(_idLength);
      memcpy(_id, rhs._id, _idLength);
    }
}

NdefRecord::~NdefRecord()
{
  if (_typeLength) free(_type);
  if (_payloadLength) free(_payload);
  if (_idLength) free(_id);
}

NdefRecord& NdefRecord::operator=(const NdefRecord& rhs)
{
  if (this != &rhs)
    {
      if (_typeLength) free(_type);
      if (_payloadLength) free(_payload);
      if (_idLength) free(_id);

      _tnf = rhs._tnf;
      _typeLength = rhs._typeLength;
      _payloadLength = rhs._payloadLength;
      _idLength = rhs._idLength;

      if (_typeLength)
        {
	  _type = (uint8_t*)malloc(_typeLength);
	  memcpy(_type, rhs._type, _typeLength);
        }

      if (_payloadLength)
        {
	  _payload = (uint8_t*)malloc(_payloadLength);
	  memcpy(_payload, rhs._payload, _payloadLength);
        }

      if (_idLength)
        {
	  _id = (uint8_t*)malloc(_idLength);
	  memcpy(_id, rhs._id, _idLength);
        }
    }
  return *this;
}

int NdefRecord::getEncodedSize()
{
  int size = 2; // tnf + typeLength
  size += (_payloadLength > 0xFF) ? 4 : 1;
  if (_idLength) size += 1;
  size += (_typeLength + _payloadLength + _idLength);
  return size;
}

void NdefRecord::encode(uint8_t *data, bool firstRecord, bool lastRecord)
{
  uint8_t* data_ptr = &data[0];
  *data_ptr = getTnfByte(firstRecord, lastRecord);
  data_ptr += 1;
  *data_ptr = _typeLength;
  data_ptr += 1;

  if (_payloadLength <= 0xFF) 
    {
      *data_ptr = _payloadLength;
      data_ptr += 1;
    } 
  else 
    {
      data_ptr[0] = 0x0;
      data_ptr[1] = 0x0;
      data_ptr[2] = (_payloadLength >> 8) & 0xFF;
      data_ptr[3] = _payloadLength & 0xFF;
      data_ptr += 4;
    }

  if (_idLength)
    {
      *data_ptr = _idLength;
      data_ptr += 1;
    }

  memcpy(data_ptr, _type, _typeLength);
  data_ptr += _typeLength;
  memcpy(data_ptr, _payload, _payloadLength);
  data_ptr += _payloadLength;

  if (_idLength)
    {
      memcpy(data_ptr, _id, _idLength);
      data_ptr += _idLength;
    }
}

uint8_t NdefRecord::getTnfByte(bool firstRecord, bool lastRecord)
{
  int value = _tnf;
  if (firstRecord) value |= 0x80;
  if (lastRecord) value |= 0x40;
  if (_payloadLength <= 0xFF) value |= 0x10;
  if (_idLength) value |= 0x8;
  return value;
}

void NdefRecord::setTnf(uint8_t tnf)
{
    _tnf = tnf;
}

void NdefRecord::print()
{
  Log.info("  NDEF Record");
  Log.info("    TNF 0x%x", _tnf);
  switch (_tnf) {
  case TNF_EMPTY: Log.info("Empty"); break;
  case TNF_WELL_KNOWN: Log.info("Well Known"); break;
  case TNF_MIME_MEDIA: Log.info("Mime Media"); break;
  case TNF_ABSOLUTE_URI: Log.info("Absolute URI"); break;
  case TNF_EXTERNAL_TYPE: Log.info("External"); break;
  case TNF_UNKNOWN: Log.info("Unknown"); break;
  case TNF_UNCHANGED: Log.info("Unchanged"); break;
  case TNF_RESERVED: Log.info("Reserved"); break;
  default: Log.info(" ");
  }
  Log.info("    Type Length 0x%x %d", _typeLength, _typeLength);
  Log.info("    Payload Length 0x%x %d", _payloadLength, _payloadLength);
  if (_idLength) Log.info("    Id Length 0x%x", _idLength);
}

// assumes the caller sized payload properly
void NdefRecord::getPayload(uint8_t *payload)
{
    memcpy(payload, _payload, _payloadLength);
}

void NdefRecord::setPayload(const uint8_t * payload, const int numBytes)
{
    if (_payloadLength)
    {
        free(_payload);
    }

    _payload = (uint8_t*)malloc(numBytes);
    memcpy(_payload, payload, numBytes);
    _payloadLength = numBytes;
}

String NdefRecord::getId()
{
  char id[_idLength + 1];
  memcpy(id, _id, _idLength);
  id[_idLength] = '\0'; // null terminate
  return String(id);
}

void NdefRecord::getId(uint8_t *id)
{
  memcpy(id, _id, _idLength);
}

void NdefRecord::setType(const uint8_t * type, const unsigned int numBytes)
{
    if(_typeLength)
    {
        free(_type);
    }

    _type = (uint8_t*)malloc(numBytes);
    memcpy(_type, type, numBytes);
    _typeLength = numBytes;
}

void NdefRecord::setId(const uint8_t * id, const unsigned int numBytes)
{
  if (_idLength)
    {
      free(_id);
    }

  _id = (uint8_t*)malloc(numBytes);
  memcpy(_id, id, numBytes);
  _idLength = numBytes;
}

/*
void NdefRecord::print()
{
  Log.info("  NDEF Record");
  Log.info("    TNF 0x%x", _tnf);
  switch (_tnf) {
  case TNF_EMPTY:
    Log.info("Empty");
    break;
  case TNF_WELL_KNOWN:
    Log.info("Well Known");
    break;
  case TNF_MIME_MEDIA:
    Log.info("Mime Media");
    break;
  case TNF_ABSOLUTE_URI:
    Log.info("Absolute URI");
    break;
  case TNF_EXTERNAL_TYPE:
    Log.info("External");
    break;
  case TNF_UNKNOWN:
    Log.info("Unknown");
    break;
  case TNF_UNCHANGED:
    Log.info("Unchanged");
    break;
  case TNF_RESERVED:
    Log.info("Reserved");
    break;
  default:
    Log.info("");
  }

  Log.info("    Type Length 0x%x %d", _typeLength, _typeLength);
  Log.info("    Payload Length 0x%x %d", _payloadLength, _payloadLength);
  if (_idLength) {
    Log.info("    Id Length 0x%x", _idLength);
  }
  Log.info("    Type ");
  PrintHexChar(_type, _typeLength);  // Assuming PrintHexChar is compatible with logging output
  Log.info("    Payload ");
  PrintHexChar(_payload, _payloadLength);
  if (_idLength) {
    Log.info("    Id ");
    PrintHexChar(_id, _idLength);
  }
  Log.info("    Record is %d bytes", getEncodedSize());


}
*/
