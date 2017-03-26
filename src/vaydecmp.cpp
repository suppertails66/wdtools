#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

// maximum size of decompressed data
const static int decompressionBufferSize = 0x20000;

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

int fromBytes(const char* bytes,
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

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

int decompressVay(char* src, char* dst) {
  
  int srcpos = 0;
  int dstpos = 0;
  
  int dataSize = fromBytes(src + srcpos,
                           4,
                           EndiannessTypes::big,
                           SignednessTypes::nosign);
  src += 4;
  
  while (srcpos < dataSize) {
    char next = src[srcpos++];
    
    for (int mask = 0x01; mask <= 0x80; mask <<= 1) {
      if (next & mask) {
        // literal
        dst[dstpos++] = src[srcpos++];
      }
      else {
        // lookback
        int offset = (unsigned char)(src[srcpos++]);
        unsigned char second = (unsigned char)(src[srcpos++]);
        offset |= ((int)(second & 0xF0)) << 4;
        int length = (second & 0x0F) + 3;
        
        // lookback offset is 0x12 less than actual
        offset += 0x12;
        // wrap around
        offset = (offset & 0xFFF);
        
        // block starts at previous 0x1000 byte boundary, or 0x0000
        // in first block
        int target;
        int blockBase;
      
        blockBase = ((dstpos / 0x1000) * 0x1000);
        int localPos = dstpos % 0x1000;
        
        if ((offset >= localPos)) {
          blockBase -= 0x1000;
        }
        
        target = blockBase + offset;
        
        // can't use memcpy -- decompressor relies on definite byte-by-byte
        // copy behavior when source and destination overlap
        for (int i = 0; i < length; i++) {
          dst[dstpos++] = dst[target + i];
        }
      }
      
      if (srcpos >= dataSize) {
        break;
      }
    }
  }
  
  return dstpos;
}

int main(int argc, char* argv[]) {
  
  if (argc < 2) {
    cout << "Usage: vaydecmp <infile> [outfile]" << endl;
    return 0;
  }
  
  string infile(argv[1]);
  
  string outfile = infile + "_dec";
  if (argc >= 3) {
    outfile = argv[2];
  }
  
  ifstream ifs (infile.c_str(), ios_base::binary);
  int infilesize = fsize(ifs);
  char* buffer = new char[infilesize];
  ifs.read(buffer, infilesize);
  
  char decompressionBuffer[decompressionBufferSize];
  int decompressedSize = decompressVay(buffer, decompressionBuffer);
  
  ofstream ofs(outfile.c_str(), ios_base::binary);
  ofs.write(decompressionBuffer, decompressedSize);
  
  delete buffer;
  
  return 0;
}
