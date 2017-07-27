#include "util/TGraphic.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>

const static int decmpBufferSize = 0x1000;
const static int decmpBufferWrap = 0xFFF;
const static int decmpBufferStartPos = 0xFEE;

const static int minimumLookbackSize = 3;
const static int maximumLookbackSize = 18;

struct BufferedFile {
  char* buffer;
  int size;
};

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

BufferedFile getFile(const char* filename) {
  std::ifstream ifs(filename, std::ios_base::binary);
  int sz = fsize(ifs);
  
  BufferedFile result;
  result.buffer = new char[sz];
  result.size = sz;
  ifs.read(result.buffer, sz);
  
  return result;
}

void saveFile(BufferedFile file, const char* filename) {
  std::ofstream ofs(filename, std::ios_base::binary);
  ofs.write(file.buffer, file.size);
}

bool fileExists(const std::string& filename) {
  std::ifstream ifs(filename.c_str(), std::ios_base::binary);
  return fsize(ifs) >= 0;
}

// Decompresses image data, returning size of the decompressed data
int decompressMglImg(const unsigned char* input,
                     int length,
                     unsigned char* outputBuf) {
  // prepare decompression buffer
  unsigned char decmpBuf[decmpBufferSize];
  std::memset(decmpBuf, 0, decmpBufferSize);
  
  int inputPos = 0;
  int decmpBufPos = decmpBufferStartPos;
  int outputBufPos = 0;
  int nextCommandByteCounter = 0;
  int commandByte = 0;
  
  // decompress the data
  while (inputPos < length) {
    // have we consumed all the bits in the command byte?
    // (the use of a bitshift counter instead of a for loop here is to more
    // closely match the structure of the original function)
    bool commandNeeded = !(nextCommandByteCounter & 0x01);
    nextCommandByteCounter >>= 1;
    
    // if so, read a new command byte
    if (commandNeeded) {
      commandByte = input[inputPos++];
      
      // break if done
      if (inputPos >= length) break;
      
      // reset counter to next command
      nextCommandByteCounter = 0x7F;
    }
    
    // get next command bit
    int command = commandByte & 0x01;
    commandByte >>= 1;
    
    // get next byte
    unsigned char byte1 = input[inputPos++];
    
    // command bit set: absolute
    if ((bool)command) {
      // write to output and decompression buffers
      outputBuf[outputBufPos++] = byte1;
      decmpBuf[decmpBufPos++] = byte1;
      
      // wrap decompression buffer
      decmpBufPos &= decmpBufferWrap;

      // break if done
      if (inputPos >= length) break;
    }
    // command bit unset: relative
    else {
      // get next byte
      unsigned char byte2 = input[inputPos++];
      
      // break if done
      if (inputPos >= length) break;
      
      // the high nybble of byte2 combines with byte1 to form a 12-bit absolute
      // offset into the decompression buffer
      int offset = ((int)(byte2 & 0xF0) << 4) | byte1;
      
      // the low nybble of byte2 is the loop count, minus 3
      int numLoops = (byte2 & 0x0F) + 3;
      
      // copy characters from decompression buffer
      for (int i = 0; i < numLoops; i++) {
        // get next char (wrapping around buffer if necessary)
        unsigned char next = decmpBuf[(offset + i) & decmpBufferWrap];
        
        // write to output and decompression buffers
        outputBuf[outputBufPos++] = next;
        decmpBuf[decmpBufPos++] = next;
        decmpBufPos &= decmpBufferWrap;
      }
    }
  }
  
  // return size of decompressed data
  return outputBufPos;
}

struct MatchResult {
  int pos;
  int len;
};

// Check how many characters match in two sequences, up to the
// maximum lookback length
MatchResult checkMatch(const unsigned char* input,
                       int length,
                       const unsigned char* check) {
  // if there are fewer characters in the input than the maximum lookback
  // size, check only those characters
  int checklen = (maximumLookbackSize > length) ? length
    : maximumLookbackSize;
  
  MatchResult result { -1, 0 };
  
  for (int i = 0; i < checklen; i++) {
    // check next position in input
    // if no match, we're done
    if (check[i] != input[i]) break;
    
    // otherwise, increment count
    ++(result.len);
  }
  
  return result;
}

// Find the best possible lookback in an input stream
MatchResult findBestMatch(const unsigned char* input,
                          int length) {
  MatchResult bestResult { -1, 0 };
  
  // check each possible starting position in buffer
  for (int i = 0; i < decmpBufferSize; i++) {
    // find length of match at this position
    MatchResult result = checkMatch(input, length,
                                    input - decmpBufferSize + i);
    
    // if match is better than current, update
    if (result.len > bestResult.len) {
      bestResult = result;
      bestResult.pos = i;
    }
  }
  
  return bestResult;
}

// Compresses image data, returning size of the compressed data
int compressMglImg(const unsigned char* rawInput,
                   int length,
                   unsigned char* outputBuf) {
  // prepare copy buffer
  // due to the way the compression works, it's easiest to make a copy
  // of the input preceded by 0x1000 null bytes
  int realLength = length + decmpBufferSize;
  unsigned char* input = new unsigned char[realLength];
  std::memset(input, 0, realLength);
  std::memcpy(input + decmpBufferSize, rawInput, length);
  
  unsigned char commandMask = 0x00;
  int commandBytePos = 0;
  
  int getpos = decmpBufferSize;
  int putpos = 0;
  while (getpos < realLength) {
    // if we've used all the bits in the current command byte, start a
    // new one
    if (commandMask == 0) {
      commandBytePos = putpos++;
      outputBuf[commandBytePos] = 0x00;
      commandMask = 0x01;
    }
    
    // find best possible match for current input
    MatchResult bestMatch = findBestMatch(input + getpos, realLength - getpos);
    
    // if the match exceeds the minimum length, add a lookback command
    if (bestMatch.len >= minimumLookbackSize) {
      // command bytes are initialized to zero, so the command bit is already
      // unset and we don't need to change it
      
      // convert input pos to buffer address
      bestMatch.pos =
        ((getpos + bestMatch.pos) - (decmpBufferSize - decmpBufferStartPos))
        & decmpBufferWrap;
      
      // prepare lookback data
      // byte 1: low 8 bits of buffer position
      // byte 2, high nybble: high 4 bits of buffer position
      // byte 2, low nybble: (numchars - 3)
      int command = (bestMatch.pos & 0x00FF);
      command |= (bestMatch.pos & 0x0F00) << 4;
      command |= (bestMatch.len - minimumLookbackSize) << 8;
      
      unsigned char byte1 = (command & 0x00FF);
      unsigned char byte2 = (command & 0xFF00) >> 8;
      
      outputBuf[putpos++] = byte1;
      outputBuf[putpos++] = byte2;
      
      // advance input pos by lookback length
      getpos += bestMatch.len;
    }
    // otherwise, add an absolute command for the next byte
    else {
      // set bit in command byte
      outputBuf[commandBytePos] |= commandMask;
      
      // add byte
      outputBuf[putpos++] = input[getpos++];
    }
    
    // move to next command bit
    commandMask <<= 1;
  }
  
  delete input;
  
  // return size of compressed data
  return putpos;
}

inline BlackT::TColor pixelToColor4bpp(int value) {
  int level = (int)(value << 4) | value;
  return BlackT::TColor(level, level, level,
                (value == 0) ? BlackT::TColor::fullAlphaTransparency
                  : BlackT::TColor::fullAlphaOpacity);
}

void placePixel(BlackT::TGraphic& dst, BlackT::TColor color, int pixelNum) {
  int x = (pixelNum % dst.w());
  int y = (pixelNum / dst.w());
  dst.setPixel(x, y, color);
}

// Read a 4bpp graphic from raw data into a TGraphic.
// The resulting TGraphic is grayscale, converting 0 -> #000000, 1 -> #111111,
// etc.
// Zero values are treated as transparent.
void read4bppGraphicGrayscale(const unsigned char* src,
                              BlackT::TGraphic& dst,
                              int width,
                              int height) {
  dst.resize(width, height);
  
  int numBytes = (width * height) / 2;
  
  for (int i = 0; i < numBytes; i++) {
    int pix1 = (*src & 0xF0) >> 4;
    int pix2 = (*src & 0x0F);
    
    BlackT::TColor color1 = pixelToColor4bpp(pix1);
    BlackT::TColor color2 = pixelToColor4bpp(pix2);
    
    placePixel(dst, color1, (i * 2));
    placePixel(dst, color2, (i * 2) + 1);
    
    src++;
  }
}

// Converts a graphic to 4bpp Saturn format, returning the size of the
// converted data.
// Input graphics are assumed to be grayscale; only the high nybble of the red
// component is actually used to determine the color index.
int writeGraphic4bpp(BlackT::TGraphic& src,
                           unsigned char* dst) {
  // parity of next write -- if true, we're targeting the low nybble
  // order is high -> low
  bool lowNyb = false;
  
  int putpos = 0;                    
  for (int j = 0; j < src.h(); j++) {
    for (int i = 0; i < src.w(); i++) {
      BlackT::TColor color = src.getPixel(i, j);
      int colorIndex = (color.r() & 0xF0) >> 4;
      
      if (lowNyb) {
        // write to low nybble and advance to next position
        dst[putpos++] |= colorIndex;
      }
      else {
        // write to high nybble (initializing low bits)
        dst[putpos] = (colorIndex << 4);
      }
      
      // flip parity
      lowNyb = !lowNyb;
    }
  }
  
  return putpos;
}
