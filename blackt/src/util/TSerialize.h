#ifndef TSERIALIZE_H
#define TSERIALIZE_H


#include "util/TByte.h"
#include "util/EndiannessType.h"
#include "util/SignednessType.h"
#include <string>
#include <vector>
#include <iostream>

namespace BlackT {


class TSerialize {
public:
  const static int u8size = 1;
  const static int u16size = 2;
  const static int u32size = 4;
  const static int u64size = 8;
  
  static int readInt(std::istream& src,
//              int& byteCount,
              int sz = u16size,
              EndiannessTypes::EndiannessType end = EndiannessTypes::little,
              SignednessTypes::SignednessType sign = SignednessTypes::nosign);
  
  static int readInt(const TByte* src,
              int& byteCount,
              int sz = u16size,
              EndiannessTypes::EndiannessType end = EndiannessTypes::little,
              SignednessTypes::SignednessType sign = SignednessTypes::nosign);
  
  static int readInt(const char* src,
              int& byteCount,
              int sz = u16size,
              EndiannessTypes::EndiannessType end = EndiannessTypes::little,
              SignednessTypes::SignednessType sign = SignednessTypes::nosign);
  
  static void writeInt(std::ostream& ofs,
               int val,
               int sz = u16size,
               EndiannessTypes::EndiannessType end = EndiannessTypes::little,
               SignednessTypes::SignednessType sign = SignednessTypes::nosign);
  
  static void writeInt(char* dst,
               int val,
               int sz = u16size,
               EndiannessTypes::EndiannessType end = EndiannessTypes::little,
               SignednessTypes::SignednessType sign = SignednessTypes::nosign);
  
  static void writeInt(TByte* dst,
               int val,
               int sz = u16size,
               EndiannessTypes::EndiannessType end = EndiannessTypes::little,
               SignednessTypes::SignednessType sign = SignednessTypes::nosign);
  
  static void writeIntS(char* dst,
               int& byteCount,
               int val,
               int sz = u16size,
               EndiannessTypes::EndiannessType end = EndiannessTypes::little,
               SignednessTypes::SignednessType sign = SignednessTypes::nosign);
  
  static void writeIntS(TByte* dst,
               int& byteCount,
               int val,
               int sz = u16size,
               EndiannessTypes::EndiannessType end = EndiannessTypes::little,
               SignednessTypes::SignednessType sign = SignednessTypes::nosign);
  
  static int readString(const TByte* src,
                        int& byteCount,
                        std::string& dst);
  
  static int readString(std::istream& ifs,
                        std::string& dst);
  
  static void writeString(std::ostream& ofs,
                          const std::string& src);
  
  template <class T>
  static void writeVectorStart(
      std::ostream& ofs,
      const std::vector<T>& vec,
      int sz = u16size,
      EndiannessTypes::EndiannessType end = EndiannessTypes::little,
      SignednessTypes::SignednessType sign = SignednessTypes::nosign) {
    writeInt(ofs, vec.size(), sz, end, sign);
  }
  
  template <class T>
  static void readVectorStart(
      std::istream& ifs,
      std::vector<T>& vec,
      int sz = u16size,
      EndiannessTypes::EndiannessType end = EndiannessTypes::little,
      SignednessTypes::SignednessType sign = SignednessTypes::nosign) {
  //  int vecSz = readInt(ifs, sz, end, sign);
    vec.resize(readInt(ifs, sz, end, sign));
  }
protected:
  
};


}


#endif
