#include "util/ByteConversion.h"

namespace BlackT {


int ByteConversion::fromBytes(const char* bytes,
                              int numBytes,
                              EndiannessTypes::EndiannessType endianness,
                              SignednessTypes::SignednessType signedness) {
  int result = 0;
                              
  switch (endianness) {
  case EndiannessTypes::little:
    for (int i = 0; i < numBytes; i++) {
      result 
        |= (((const unsigned char*)bytes)[i]) << (i * 8);
    }
    break;
  case EndiannessTypes::big:
    for (int i = 0; i < numBytes; i++) {
      result 
        |= (((const unsigned char*)bytes)[i]) << ((numBytes - i - 1) * 8);
    }
    break;
  default:
    break;
  }
  
  // Convert signed values from two's complement
  if (signedness == SignednessTypes::sign) {
    // If highest bit is set, number is negative
    int shift = (1 << ((numBytes * 8) - 1));
    if (result & shift) {
      result -= (shift << 1);
    }
  }
  
  return result;
}
                       
void ByteConversion::toBytes(int value,
                             char* dest,
                             int numBytes,
                             EndiannessTypes::EndiannessType endianness,
                             SignednessTypes::SignednessType signedness) {
  
  // Convert negative values to native two's complement representation
  if (value < 0
      && signedness == SignednessTypes::sign) {
    value += ((1 << (numBytes * 8)));
  }
  
  switch (endianness) {
  case EndiannessTypes::little:
    for (int i = 0; i < numBytes; i++) {
      int shift = (i * 8);
      ((unsigned char*)dest)[i]
        = (unsigned char)((value & (0xFF << shift)) >> shift);
    }
    break;
  case EndiannessTypes::big:
    for (int i = 0; i < numBytes; i++) {
      int shift = (i * 8);
      ((unsigned char*)dest)[numBytes - i - 1]
        = (unsigned char)((value & (0xFF << shift)) >> shift);
    }
    break;
  default:
    break;
  }
}
                      
int ByteConversion::fromBytes(const unsigned char* bytes,
                     int numBytes,
                     EndiannessTypes::EndiannessType endianness,
                     SignednessTypes::SignednessType signedness) {
  return ByteConversion::fromBytes((const char*)bytes,
                            numBytes,
                            endianness,
                            signedness);
}
                     
void ByteConversion::toBytes(int value,
                    unsigned char* dest,
                    int numBytes,
                    EndiannessTypes::EndiannessType endianness,
                    SignednessTypes::SignednessType signedness) {
  ByteConversion::toBytes(value,
                          (char*)dest,
                          numBytes,
                          endianness,
                          signedness);
}


}; 
