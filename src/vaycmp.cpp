#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>

// maximum size of compressed data
const static int compressionBufferSize = 0x20000;

// compression constants
const static int minLookbackLength = 3;
const static int maxLookbackLength = 19;
const static int blockSize = 0x1000;

using namespace std;

namespace SignednessTypes {
  enum SignednessType {
    nosign = 0,
    sign = 1
  };
};

namespace EndiannessTypes {
  enum EndiannessType {
    little = 0,
    big
  };
};
                       
void toBytes(int value,
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

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

struct LookbackResult {
  int offset;
  int size;
};

LookbackResult findBestLookback(char* src, int srclen, int srcpos) {
  LookbackResult result = { 0, 0 };
  
  int maxlen = std::min(maxLookbackLength, srclen - srcpos);
  int limit = std::max(srcpos - blockSize, 0);
  
  for (int len = minLookbackLength; len < maxlen; len++) {
    
    // Search backwards from starting position for matches
    bool found = false;
    for (int j = srcpos - len; j >= limit; j--) {
      if (std::memcmp(src + j, src + srcpos, len) == 0) {
        result.offset = (j - srcpos);
        result.size = len;
        found = true;
        break;
      }
    }
    
    // If we failed to find a match, we don't need to test for longer matches
    if (!found) {
      break;
    }
    
  }
  
  return result;
}

int compressVay(char* src, int srclen, char* dst) {
  
  int srcpos = 0;
  
  // Reserve space for compressed size field
  int dstpos = 4;
  
  while (srcpos < srclen) {
    // Reserve space for compression byte
    dst[dstpos] = 0;
    int compressionBytePos = dstpos++;
    
    // Build a sequence of (up to) 8 compression commands to put in the
    // compression byte
    for (int mask = 0x01; mask <= 0x80; mask <<= 1) {
      
      // Find the position of the longest matching lookback for the current
      // input
      LookbackResult bestLookback = findBestLookback(src, srclen, srcpos);
      
      // If lookback failed, add a literal and move on
      if (bestLookback.size < minLookbackLength) {
        dst[dstpos++] = src[srcpos++];
        dst[compressionBytePos] |= mask;
      }
      // Otherwise, convert lookback to expected format
      else {
        // Lookback offsets are "absolute", based on 0x1000 byte blocks.
        // They work like this:
        // If we're at position 0x1500, and we encounter a lookback with
        // offset 0x400, then we should get the data from address 0x1400.
        // If we're at the same position and encounter a lookback with
        // offset 0x600, we should get the data from 0x0600, since otherwise
        // we'd exceed our current position.
        // Offsets are simple to compute -- just take the absolute address
        // modulo 0x1000.
        
        // Compute the literal offset of the data we want
        int targetOffset = (srcpos + bestLookback.offset) % blockSize;
        
        // Subtract constant 0x12 (for some reason), wrapping around the
        // block if necessary
        targetOffset -= 0x12;
        if (targetOffset < 0) {
          targetOffset += blockSize;
        }
        
        // Convert to compression command
        dst[dstpos++] = (targetOffset & 0xFF);
        dst[dstpos] |= (targetOffset & 0xF00) >> 4;
        dst[dstpos++] |= (bestLookback.size - minLookbackLength) & 0x0F;
        
        // Advance srcpos
        srcpos += bestLookback.size;
        
        // Compression bit is pre-cleared
      }
      
      // Check if we're done
      if (srcpos >= srclen) {
        break;
      }
    }
    
  }
  
  // Set compressed data size (not including the size field itself)
  toBytes(dstpos - 4, dst, 4,
          EndiannessTypes::big,
          SignednessTypes::nosign);
  
  return dstpos;
}

int main(int argc, char* argv[]) {
  
  if (argc < 2) {
    cout << "Usage: vaycmp <infile> [outfile]" << endl;
    return 0;
  }
  
  string infile(argv[1]);
  
  string outfile = infile + "_cmp";
  if (argc >= 3) {
    outfile = argv[2];
  }
  
  ifstream ifs (infile.c_str(), ios_base::binary);
  int infilesize = fsize(ifs);
  char* buffer = new char[infilesize];
  ifs.read(buffer, infilesize);
  
  char compressionBuffer[compressionBufferSize];
  int compressedSize = compressVay(buffer, infilesize, compressionBuffer);
  
  ofstream ofs(outfile.c_str(), ios_base::binary);
  ofs.write(compressionBuffer, compressedSize);
  
  delete buffer;
  
  return 0;
}
