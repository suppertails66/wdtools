#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "util/ByteConversion.h"

// maximum size of decompressed data
const static int decompressionBufferSize = 0x40000;

using namespace std;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

enum ExeDecompressionCommand {
  exeLiteral,
  exeLookback1b,
  exeLookback2b
};

int exeGetNextBit(const char* src, int& getpos,
              int& cmdpos, int& mask) {
  // ready new command byte from stream if needed
  if (mask > 0x80) {
    cmdpos = getpos++;
    mask = 0x01;
  }
  
  int result = src[cmdpos] & mask;
  mask <<= 1;
  
  return (result != 0);
}

ExeDecompressionCommand exeGetCommand(const char* src, int& getpos,
                                      int& cmdpos, int& mask) {
  if (exeGetNextBit(src, getpos, cmdpos, mask)) return exeLiteral;
  if (exeGetNextBit(src, getpos, cmdpos, mask)) return exeLookback1b;
  return exeLookback2b;
}

void exeDecompressNext(const char* src, char* dst, int sz,
                       int& getpos, int& cmdpos, int& dstpos, int& mask) {
  
  ExeDecompressionCommand cmd = exeGetCommand(src, getpos, cmdpos, mask);
  
  switch (cmd) {
  case exeLiteral:
    dst[dstpos++] = src[getpos++];
    break;
  case exeLookback1b:
    {
      unsigned char lookbyte = src[getpos++];
      int length = ((lookbyte & 0xC0) >> 6) + 2;
      int lookbackPos = -(0x100 - (int)(lookbyte | 0xC0)) + dstpos;
      
//      if (lookbackPos < 0) cerr << "1: " << getpos << " " << cmdpos << endl;
      if (lookbackPos < 0) { cerr << "1: " << getpos << " " << cmdpos << endl; getpos = sz; return; }
      
//      memcpy(dst + dstpos, dst + dstpos + distance, distance);
      // allow for overlapping
      for (int i = 0; i < length; i++) {
        dst[dstpos++] = dst[lookbackPos++];
        if (dstpos >= sz) break;
      }
    }
    break;
  case exeLookback2b:
    {
      int lookback = 0;
      unsigned char next = src[getpos++];
      unsigned char next2 = src[getpos++];
      lookback |= (int)(next) << 8;
      lookback |= (int)(next2);
      int length = (lookback & 0xF000) >> 12;
      
/*      cout << lookback << endl;
      cout << length << endl;
      cout << getpos << endl;
      cout << cmdpos << endl;
      char c; cin >> c; */
      
      if (length == 0) {
        int length = (unsigned char)(src[getpos++]);
        if (length == 0) {
          // there seems to be code to do some sort of esoteric read
          // for this command, but this never occurs in valid data
          cerr << "reached zero case at " << hex << getpos
            << " (" << cmdpos << ")" << endl;
          getpos = sz;
//          exit(1);
        }
        else {
          length += 2;
          int lookbackPos = -(0x10000 - (int)(lookback | 0xF000)) + dstpos;
      if (lookbackPos < 0) { cerr << "2: " << getpos << " " << cmdpos << endl; getpos = sz; return; }
          for (int i = 0; i < length; i++) {
            dst[dstpos++] = dst[lookbackPos++];
            if (dstpos >= sz) break;
          }
        }
      }
      else {
        length += 2;
        int lookbackPos = -(0x10000 - (int)(lookback | 0xF000)) + dstpos;
      if (lookbackPos < 0) { cerr << "3: " << getpos << " " << cmdpos << endl; getpos = sz; return; }
        for (int i = 0; i < length; i++) {
          dst[dstpos++] = dst[lookbackPos++];
          if (dstpos >= sz) break;
        }
      }
    }
    break;
  default:
    cerr << "?????" << endl;
    break;
  }
}

int decompressGearboltExe(const char* src, char* dst, int sz) {
  
  int getpos = 0;   // current overall fetch pos
  int cmdpos = 0;   // current compression byte pos
  int dstpos = 0;
  int mask = 0x100;
  
  while (getpos < sz) {
    exeDecompressNext(src, dst, sz, getpos, cmdpos, dstpos, mask);
  }
  
  return dstpos;
}

int main(int argc, char* argv[]) {
  
  if (argc < 3) return 0;
  
  ifstream ifs(argv[1], ios_base::binary);
  int sz = fsize(ifs);
  char* buffer = new char[sz];
  
  ifs.read(buffer, sz);
  
  char decmpBuffer[decompressionBufferSize];
  int decmpSz = decompressGearboltExe(buffer, decmpBuffer, sz);
  
  ofstream ofs(argv[2], ios_base::binary);
  ofs.write(decmpBuffer, decmpSz);
  
  delete buffer;
  
}
