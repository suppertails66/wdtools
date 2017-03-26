#include "util/TSerialize.h"
#include "util/ByteConversion.h"

namespace BlackT {


int TSerialize::readInt(std::istream& src,
//              int& byteCount,
              int sz,
              EndiannessTypes::EndiannessType end,
              SignednessTypes::SignednessType sign) {
  TByte buffer[16];
  src.read((char*)buffer, sz);
  
  return ByteConversion::fromBytes(
                      buffer, sz,
                      end, sign);
}

int TSerialize::readInt(const TByte* src,
              int& byteCount,
              int sz,
              EndiannessTypes::EndiannessType end,
              SignednessTypes::SignednessType sign) {
  
  int result = ByteConversion::fromBytes(
                      src + byteCount, sz,
                      end, sign);
  byteCount += sz;
  
  return result;
}

int TSerialize::readInt(const char* src,
              int& byteCount,
              int sz,
              EndiannessTypes::EndiannessType end,
              SignednessTypes::SignednessType sign) {
  return readInt((const TByte*)src, byteCount, sz, end, sign);
}
  
void TSerialize::writeInt(std::ostream& ofs,
             int val,
             int sz,
              EndiannessTypes::EndiannessType end,
              SignednessTypes::SignednessType sign) {
  char buffer[16];
  ByteConversion::toBytes(val, buffer, sz,
                          end, sign);
  ofs.write(buffer, sz);
}
  
void TSerialize::writeInt(char* dst,
             int val,
             int sz,
              EndiannessTypes::EndiannessType end,
              SignednessTypes::SignednessType sign) {
  ByteConversion::toBytes(val, dst, sz,
                          end, sign);
}
  
void TSerialize::writeInt(TByte* dst,
             int val,
             int sz,
              EndiannessTypes::EndiannessType end,
              SignednessTypes::SignednessType sign) {
  writeInt((char*)dst, val, sz, end, sign);
}
  
void TSerialize::writeIntS(char* dst,
             int& byteCount,
             int val,
             int sz,
             EndiannessTypes::EndiannessType end,
             SignednessTypes::SignednessType sign) {
  writeInt(dst + byteCount, val, sz, end, sign);
  byteCount += sz;
}

void TSerialize::writeIntS(TByte* dst,
             int& byteCount,
             int val,
             int sz,
             EndiannessTypes::EndiannessType end,
             SignednessTypes::SignednessType sign) {
  writeIntS((char*)dst, byteCount, val, sz, end, sign);
}
  
int TSerialize::readString(const TByte* src,
                      int& byteCount,
                      std::string& dst) {
  int sz = readInt(src, byteCount, TSerialize::u16size,
                   EndiannessTypes::little, SignednessTypes::nosign);
  
  dst = std::string((const char*)(src + byteCount), sz);
  byteCount += sz;
  
  return sz + TSerialize::u16size;
}
  
int TSerialize::readString(std::istream& ifs,
                      std::string& dst) {
  int sz = readInt(ifs, TSerialize::u16size,
                   EndiannessTypes::little, SignednessTypes::nosign);
  char* buffer = new char[sz];
  ifs.read(buffer, sz);
  dst.assign(buffer, sz);
  delete buffer;
  
  return sz + TSerialize::u16size;
}

void TSerialize::writeString(std::ostream& ofs,
                        const std::string& src) {
  writeInt(ofs, src.size(), TSerialize::u16size,
           EndiannessTypes::little, SignednessTypes::nosign);
  ofs.write(src.c_str(), src.size());
}


} 
