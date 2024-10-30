#include "Ndef.h"

// Borrowed from Adafruit_NFCShield_I2C
void PrintHex(const uint8_t *data, const long numBytes)
{
  for (long szPos = 0; szPos < numBytes; szPos++)
    {
      if (data[szPos] <= 0xF)
	Log.info("0x0%l0X", data[szPos]);
      else
	Log.info("0x%lX", data[szPos]);
    }
}

// Borrowed from Adafruit_NFCShield_I2C
void PrintHexChar(const uint8_t *data, const long numBytes)
{
  String hexOutput = "";
  String charOutput = "";

  for (long szPos = 0; szPos < numBytes; szPos++)
    {
      if (data[szPos] <= 0xF)
	hexOutput += "0";
      hexOutput += String::format("%X ", data[szPos]);

      if (data[szPos] <= 0x1F)
	charOutput += ".";
      else
	charOutput += (char)data[szPos];
    }
  Log.info("%s  %s", hexOutput.c_str(), charOutput.c_str());
}

// Note if buffer % blockSize != 0, last block will not be written
void DumpHex(const uint8_t *data, const long numBytes, const unsigned int blockSize)
{
  for (int i = 0; i < (numBytes / blockSize); i++)
    {
      PrintHexChar(data, blockSize);
      data += blockSize;
    }
}
