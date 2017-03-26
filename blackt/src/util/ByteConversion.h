#ifndef BYTECONVERION_H
#define BYTECONVERION_H


#include "util/EndiannessType.h"
#include "util/SignednessType.h"

// this include isn't necessary but I'm damned fed up with having to
// retype it for every one of the eleven bajillion files I've used it in
#include "util/ByteSizes.h"

namespace BlackT {


class ByteConversion {
public:
  static int fromBytes(const char* bytes,
                       int numBytes,
                       EndiannessTypes::EndiannessType endianness,
                       SignednessTypes::SignednessType signedness);
                       
  static void toBytes(int value,
                      char* dest,
                      int numBytes,
                      EndiannessTypes::EndiannessType endianness,
                      SignednessTypes::SignednessType signedness);
                      
  static int fromBytes(const unsigned char* bytes,
                       int numBytes,
                       EndiannessTypes::EndiannessType endianness,
                       SignednessTypes::SignednessType signedness);
                       
  static void toBytes(int value,
                      unsigned char* dest,
                      int numBytes,
                      EndiannessTypes::EndiannessType endianness,
                      SignednessTypes::SignednessType signedness);
protected:
};


};


#endif 
