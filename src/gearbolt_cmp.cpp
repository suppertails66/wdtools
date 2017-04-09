#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "util/ByteConversion.h"

// maximum size of compressed data
const static int compressionBufferSize = 0x100000;

using namespace std;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

enum ExeCompressionCommand {
  exeLiteral,
  exeLookback1b,
  exeLookback2b
};

const static int min1bLookback = 2;
const static int max1bLookback = 5;
const static int max1bLookbackDistance = 0x40;

const static int min2bShortLookback = 3;
const static int max2bShortLookback = 17;
const static int max2bShortLookbackDistance = 0x1000;

const static int min2bLongLookback = 4;
const static int max2bLongLookback = 257;
const static int max2bLongLookbackDistance = 0x1000;

int checkLookbackLength(const char* src, int sz, int checkpos, int getpos,
                        int minlen, int maxlen) {
  int len = 0;
  int limit = ((checkpos + maxlen) > sz) ? (sz - checkpos)
                  : maxlen;
  
  for (int i = 0; i < limit; i++) {
    if (src[checkpos + i] != src[getpos + i]) break;
    ++len;
  }
  
  if (len < minlen) return -1;
  return len;
}

/*int check1bLookbackLength(const char* src, int sz, int checkpos, int getpos) {
  int len = 0;
  int limit = ((getpos + max1bLookback) > sz) ? (sz - getpos)
                  : max1bLookback;
  
  for (int i = 0; i < limit; i++) {
    if (src[checkpos + i] != src[getpos + i]) break;
    ++len;
  }
  
  if (len < min1bLookback) return -1;
  return len;
} */

int findBestLookbackPos(const char* src, int sz, int getpos,
                        int maxLookbackDistance, int minlen, int maxlen) {
  int bestlen = -1;
  int bestpos = -1;
  int start = getpos - maxLookbackDistance;
  if (start <= 0) start = 0;
  int end = getpos;
  
  for (int i = start; i < end; i++) {
    int nextlen = checkLookbackLength(src, sz, i, getpos,
                minlen, maxlen);
    if (nextlen > bestlen) {
      bestpos = i;
      bestlen = nextlen;
    }
  }
  
  return bestpos;
}

/*int findBest1bLookbackPos(const char* src, int sz, int getpos) {
  int bestlen = -1;
  int bestpos = -1;
  int start = getpos - max1bLookbackDistance;
  if (start <= 0) start = 0;
  int end = getpos;
  
  for (int i = start; i < end; i++) {
    int nextlen = checkLookbackLength(src, sz, i, getpos,
                min1bLookback, max1bLookback);
    if (nextlen > bestlen) bestlen = nextlen;
  }
  
  return bestpos;
} */

void exeWriteBit(char* dst, int& putpos,
                int& cmdpos, int& mask,
                int value) {
  // ready new command byte from stream if needed
  if (mask > 0x80) {
    cmdpos = putpos++;
    dst[cmdpos] = 0;
    mask = 0x01;
  }
  
  if (value) dst[cmdpos] |= mask;
  mask <<= 1;
}

void exeWriteCommand(char* dst, int& putpos,
                int& cmdpos, int& mask,
                ExeCompressionCommand command) {
  switch (command) {
  case exeLiteral:
    exeWriteBit(dst, putpos, cmdpos, mask, 1);
    break;
  case exeLookback1b:
    exeWriteBit(dst, putpos, cmdpos, mask, 0);
    exeWriteBit(dst, putpos, cmdpos, mask, 1);
    break;
  case exeLookback2b:
    exeWriteBit(dst, putpos, cmdpos, mask, 0);
    exeWriteBit(dst, putpos, cmdpos, mask, 0);
    break;
  default:
    cerr << "???????" << endl;
    break;
  }
}

void doNextCompression(const char* src, int sz, char* dst,
                       int& getpos, int& putpos, int& cmdpos,
                       int& mask) {
  
  // find longest possible 1b lookback
  // find longest possible 2b lookback
  // which gives us best efficiency?
  // if 2b, make sure we adjust accordingly if a long lookback
  // if nothing is efficient enough, do absolute
  
  int best1bLookback = findBestLookbackPos(src, sz, getpos,
                                           max1bLookbackDistance,
                                           min1bLookback,
                                           max1bLookback);
  int best2bLookback = findBestLookbackPos(src, sz, getpos,
                                           max2bLongLookbackDistance,
                                           min2bShortLookback,
                                           max2bLongLookback);
    
  int best1bLooklen = (best1bLookback != -1) ?
          checkLookbackLength(src, sz, best1bLookback, getpos,
                                      min1bLookback,
                                      max1bLookback)
          : -1;
  int best2bLooklen =  (best2bLookback != -1) ?
          checkLookbackLength(src, sz, best2bLookback, getpos,
                                    min2bShortLookback,
                                    max2bLongLookback)
          : -1;
  
//  cout << dec << best1bLooklen << " " << best2bLooklen << endl;
//  cout << "pos: " << hex << getpos << endl;
  if ((best1bLookback == -1) && (best2bLookback == -1)) {
    // absolute
//    cout << "1\n";
    exeWriteCommand(dst, putpos, cmdpos, mask, exeLiteral);
    dst[putpos++] = src[getpos++];
  }
  else if (best2bLooklen > best1bLooklen) {
    // 2b lookback
    exeWriteCommand(dst, putpos, cmdpos, mask, exeLookback2b);
    
    int looklen = best2bLooklen;
    
    if (looklen > max2bShortLookback) {
//      cout << "2a: " << hex << best2bLookback << ", " << looklen << "\n";
      // long lookback -- length must be 0
      int base = 0x0000;
      int offset = getpos - best2bLookback;
      base |= (0x0FFF & (max2bLongLookbackDistance - offset));
      dst[putpos++] = (base & 0xFF00) >> 8;
      dst[putpos++] = (base & 0x00FF);
      dst[putpos++] = (unsigned char)(looklen - 2);
    }
    else {
//      cout << "2b: " << hex << best2bLookback << ", " << looklen << "\n";
      // short lookback
      int base = ((looklen - 2) << 12) & 0xF000;
      int offset = getpos - best2bLookback;
      base |= (0x0FFF & (max2bShortLookbackDistance - offset));
      dst[putpos++] = (base & 0xFF00) >> 8;
      dst[putpos++] = (base & 0x00FF);
    }
    
    getpos += looklen;
  }
  else {
    // 1b lookback
    exeWriteCommand(dst, putpos, cmdpos, mask, exeLookback1b);
    
    int looklen = best1bLooklen;
//    cout << "3: " << hex << best1bLookback << ", " << looklen << "\n";
    
    int base = ((looklen - 2) << 6) & 0xC0;
    int offset = getpos - best1bLookback;
    base |= (0x3F & (max1bLookbackDistance - offset));
    dst[putpos++] = (base & 0xFF);
    
    getpos += looklen;
  }
}

int exeCompress(const char* src, int sz, char* dst) {
  int getpos = 0;
  int putpos = 0;
  int cmdpos = 0;
  int mask = 0x100;
  
  while (getpos < sz) {
    doNextCompression(src, sz, dst, getpos, putpos, cmdpos, mask);
  }
  
  // write end-of-file
  exeWriteCommand(dst, putpos, cmdpos, mask, exeLookback2b);
  dst[putpos++] = 0;
  dst[putpos++] = 0;
  dst[putpos++] = 0;
  
  return putpos;
}

/*int exeGetNextBit(const char* src, int& getpos,
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

ExeCompressionCommand exeGetCommand(const char* src, int& getpos,
                                      int& cmdpos, int& mask) {
  if (exeGetNextBit(src, getpos, cmdpos, mask)) return exeLiteral;
  if (exeGetNextBit(src, getpos, cmdpos, mask)) return exeLookback1b;
  return exeLookback2b;
}

void exeCompressNext(const char* src, char* dst, int sz,
                       int& getpos, int& cmdpos, int& dstpos, int& mask) {
  
  ExeCompressionCommand cmd = exeGetCommand(src, getpos, cmdpos, mask);
  
  switch (cmd) {
  case exeLiteral:
    dst[dstpos++] = src[getpos++];
    break;
  case exeLookback1b:
    {
      unsigned char lookbyte = src[getpos++];
      int length = ((lookbyte & 0xC0) >> 6) + 2;
      int lookbackPos = -(0x100 - (int)(lookbyte | 0xC0)) + dstpos;
      
      // allow for overlapping
      for (int i = 0; i < length; i++) {
        dst[dstpos++] = dst[lookbackPos++];
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
          for (int i = 0; i < length; i++) {
            dst[dstpos++] = dst[lookbackPos++];
//            if (dstpos >= sz) break;
          }
        }
      }
      else {
        length += 2;
        int lookbackPos = -(0x10000 - (int)(lookback | 0xF000)) + dstpos;
        for (int i = 0; i < length; i++) {
          dst[dstpos++] = dst[lookbackPos++];
//          if (dstpos >= sz) break;
        }
      }
    }
    break;
  default:
    cerr << "?????" << endl;
    break;
  }
}

int compressGearboltExe(const char* src, char* dst, int sz) {
  
  int getpos = 0;   // current overall fetch pos
  int cmdpos = 0;   // current compression byte pos
  int dstpos = 0;
  int mask = 0x100;
  
  while (getpos < sz) {
    exeCompressNext(src, dst, sz, getpos, cmdpos, dstpos, mask);
  }
  
  return dstpos;
} */

int main(int argc, char* argv[]) {
  
  if (argc < 3) return 0;
  
  ifstream ifs(argv[1], ios_base::binary);
  int sz = fsize(ifs);
//  int realSz = ((sz / 0x800) * 0x800) + 0x800;
//  char* buffer = new char[realSz];
  char* buffer = new char[sz];
  ifs.read(buffer, sz);
  
  char cmpBuffer[compressionBufferSize];
//  int cmpSz = compressGearboltExe(buffer, cmpBuffer, sz);
  int cmpSz = exeCompress(buffer, sz, cmpBuffer);
  
  int realCmpSz = ((cmpSz / 0x800) * 0x800) + 0x800;
  
  // zero-pad to nearest sector boundary
  memset(cmpBuffer + cmpSz, 0, realCmpSz - cmpSz);
  cmpSz = realCmpSz;
  
  ofstream ofs(argv[2], ios_base::binary);
  ofs.write(cmpBuffer, cmpSz);
  
  delete buffer;
  
}
