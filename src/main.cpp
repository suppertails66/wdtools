#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TStringConversion.h"
#include "util/ByteConversion.h"
#include "util/TTwoDArray.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace BlackT;

const static int patternW = 8;
const static int patternH = 8;
const static int patternsPerRow = 16;
const static int bytesPerPattern = 32;
const static int rowWidth = patternW * patternsPerRow;
const static int rowHeight = patternH;

const static TByte standardPaletteLineDataSample[33]
  = "\x00\x40\x00\x00\x08\x20\x0C\x60\x00\x04"
    "\x00\x28\x00\x4C\x04\x6A\x08\xAC\x00\xCC"
    "\x0C\x8E\x00\x20\x00\x80\x06\x66\x0A\xAA"
    "\x0C\xCC";

const static TByte standardPaletteLineData[33]
  = "\x00\x40\x00\x00\x08\x20\x0C\x60\x00\x04"
    "\x00\x28\x00\x4C\x04\x6A\x08\xAC\x00\xCC"
    "\x0C\x8E\x00\x20\x00\x80\x06\x66\x0A\xAA"
    "\x0C\xCC";

struct MdColor {

  MdColor()
    : r(0), g(0), b(0) { }
  
  MdColor(int rawColor)
    : r(0), g(0), b(0) {
    r = (rawColor & 0x000E) >> 1;
    g = (rawColor & 0x00E0) >> 5;
    b = (rawColor & 0x0E00) >> 9;
  }
  
  TColor toColor() {
//    return TColor(r << 5, g << 5, b << 5,
//                  !(r && g && b)
//                    ? TColor::fullAlphaTransparency
//                    : TColor::fullAlphaOpacity);
    return TColor(r << 5, g << 5, b << 5,
                  TColor::fullAlphaOpacity);
  }
  
  int r;
  int g;
  int b;
  
};

struct MdPaletteLine {
  
  const static int numColors = 16;
  const static int uncompressedSize = numColors * 2;
  
  MdPaletteLine() { }
  
  MdPaletteLine(const TByte* src) {
    for (int i = 0; i < numColors; i++) {
      int raw = ByteConversion::fromBytes(src + (i * 2),
                                           ByteSizes::uint16Size,
                                           EndiannessTypes::big,
                                           SignednessTypes::nosign);
      
      colors[i] = MdColor(raw);
    }
  }
  
  TColor colorAtIndex(int index) {
    TColor col = colors[index].toColor();
    
    if (index == 0) {
      col.setA(TColor::fullAlphaTransparency);
    }
    
    return col;
  }
  
  void generatePreview(TGraphic& dst, int colorW = 16, int colorH = 16) {
    dst.resize(colorW * numColors, colorH);
  
    for (int i = 0; i < numColors; i++) {
      int xPos = i * colorW;
      
      TColor color = colorAtIndex(i);
      
      dst.fillRect(xPos, 0,
                   colorW, colorH,
                   color);
    }
  }
  
  MdColor colors[numColors];
};

struct MdPalette {
  const static int numLines = 4;
  
  MdPalette() { }
  
  MdPalette(const TByte* src) {
    for (int i = 0; i < numLines; i++) {
      lines[i] = MdPaletteLine(src);
      src += MdPaletteLine::uncompressedSize;
    }
  }
  
  MdPaletteLine lines[numLines];
};

struct MdPattern {
  typedef TTwoDArray<TByte> PatternArray;
  
  const static int w = 8;
  const static int h = 8;
  const static int uncompressedSize = 32;
  
  MdPattern()
    : pattern(w, h) { }
    
  void toGrayscaleGraphic(TGraphic& dst) {
    dst.resize(w, h);
    
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        int raw = pattern.data(i, j);
        TColor color = TColor(raw * 16, raw * 16, raw * 16,
                      (raw == 0) ? TColor::fullAlphaTransparency
                                 : TColor::fullAlphaOpacity);
        
        dst.setPixel(i, j, color);
      }
    }
  }
    
  void toColorGraphic(TGraphic& dst, MdPaletteLine line) {
    dst.resize(w, h);
    
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        int raw = pattern.data(i, j);
        TColor color = line.colorAtIndex(raw);
        dst.setPixel(i, j, color);
      }
    }
  }
  
  PatternArray pattern;
  
};

const static MdPaletteLine StandardPalette(
    standardPaletteLineData);
const static MdPaletteLine StandardPaletteSample(
    standardPaletteLineDataSample);

void readPattern(MdPattern& dst, char* src) {
  int pos = 0;
  for (int j = 0; j < MdPattern::h; j++) {
    for (int i = 0; i < MdPattern::w; i += 2) {
      unsigned char b = (unsigned char)(src[pos++]);
      
      int second = (b & 0x0F);
      int first = (b & 0xF0) >> 4;
      
      dst.pattern.data(i, j) = first;
      dst.pattern.data(i + 1, j) = second;
    }
  }
}

void readPattern(TGraphic& dst, char* src) {
  dst.clear();
  
  int pos = 0;
  for (int j = 0; j < patternH; j++) {
    for (int i = 0; i < patternW; i += 2) {
      unsigned char b = (unsigned char)(src[pos++]);
      
      int second = (b & 0x0F);
      int first = (b & 0xF0) >> 4;
      
      dst.setPixel(i, j, TColor(first * 16,
                                first * 16,
                                first * 16,
                                (first == 0)
                                  ? TColor::fullAlphaTransparency
                                  : TColor::fullAlphaOpacity));
      
      dst.setPixel(i + 1, j, TColor(second * 16,
                                second * 16,
                                second * 16,
                                (second == 0)
                                  ? TColor::fullAlphaTransparency
                                  : TColor::fullAlphaOpacity));
    }
  }
  
  dst.regenerateTransparencyModel();
}

struct ColorStrip {
  ColorStrip()
    : isReal(false),
      r0(false),
      r1(false),
      r2(false),
      r3(false),
      l0(false),
      l1(false),
      l2(false),
      l3(false) { }
  ColorStrip(unsigned char src)
    : isReal(true),
      r0((src & 0x01) != 0),
      r1((src & 0x02) != 0),
      r2((src & 0x04) != 0),
      r3((src & 0x08) != 0),
      l0((src & 0x10) != 0),
      l1((src & 0x20) != 0),
      l2((src & 0x40) != 0),
      l3((src & 0x80) != 0) { }

  bool isReal;
  bool r0;
  bool r1;
  bool r2;
  bool r3;
  bool l0;
  bool l1;
  bool l2;
  bool l3;
};

struct StripAggregate {
  
  typedef std::vector<ColorStrip> StripCollection;
  
  StripCollection stripsU0;
  StripCollection stripsU1;
  StripCollection stripsU2;
  StripCollection stripsU3;
  
  StripCollection stripsD0;
  StripCollection stripsD1;
  StripCollection stripsD2;
  StripCollection stripsD3;
  
  void toTile(TGraphic& dst, bool invert = false) {
    dst.clear();
  
    processStrips(dst, stripsU0, 0, 0, invert);
    processStrips(dst, stripsU1, 2, 0, invert);
    processStrips(dst, stripsU2, 4, 0, invert);
    processStrips(dst, stripsU3, 6, 0, invert);
    
    processStrips(dst, stripsD0, 0, 4, invert);
    processStrips(dst, stripsD1, 2, 4, invert);
    processStrips(dst, stripsD2, 4, 4, invert);
    processStrips(dst, stripsD3, 6, 4, invert);
  
    dst.regenerateTransparencyModel();
  }
  
  void toMdPattern(MdPattern& dst, bool invert = false) {
    processStrips(dst, stripsU0, 0, 0, invert);
    processStrips(dst, stripsU1, 2, 0, invert);
    processStrips(dst, stripsU2, 4, 0, invert);
    processStrips(dst, stripsU3, 6, 0, invert);
    
    processStrips(dst, stripsD0, 0, 4, invert);
    processStrips(dst, stripsD1, 2, 4, invert);
    processStrips(dst, stripsD2, 4, 4, invert);
    processStrips(dst, stripsD3, 6, 4, invert);
  }
  
protected:

  void processStrips(TGraphic& dst,
                     StripCollection& strips,
                     int xOffset,
                     int yOffset,
                     bool invert = false) {
/*    for (int i = 0; i < strips.size(); i++) {
      ColorStrip& strip = strips[i];
      std::cout << strip.r0 << " "
                << strip.r1 << " "
                << strip.r2 << " "
                << strip.r3 << " "
                << strip.l0 << " "
                << strip.l1 << " "
                << strip.l2 << " "
                << strip.l3 << std::endl;
    }
    std::cout << std::endl; */
                    
    dst.setPixel(xOffset + 1,
                 yOffset + 0,
                 getColor(strips, 0, invert));
    dst.setPixel(xOffset + 1,
                 yOffset + 1,
                 getColor(strips, 1, invert));
    dst.setPixel(xOffset + 1,
                 yOffset + 2,
                 getColor(strips, 2, invert));
    dst.setPixel(xOffset + 1,
                 yOffset + 3,
                 getColor(strips, 3, invert));
  
    dst.setPixel(xOffset + 0,
                 yOffset + 0,
                 getColor(strips, 4, invert));
    dst.setPixel(xOffset + 0,
                 yOffset + 1,
                 getColor(strips, 5, invert));
    dst.setPixel(xOffset + 0,
                 yOffset + 2,
                 getColor(strips, 6, invert));
    dst.setPixel(xOffset + 0,
                 yOffset + 3,
                 getColor(strips, 7, invert));
  }

  void processStrips(MdPattern& dst,
                     StripCollection& strips,
                     int xOffset,
                     int yOffset,
                     bool invert = false) {
                    
    dst.pattern.data(xOffset + 1,
                 yOffset + 0)
                 = getRawColor(strips, 0, invert);
    dst.pattern.data(xOffset + 1,
                 yOffset + 1)
                 = getRawColor(strips, 1, invert);
    dst.pattern.data(xOffset + 1,
                 yOffset + 2)
                 = getRawColor(strips, 2, invert);
    dst.pattern.data(xOffset + 1,
                 yOffset + 3)
                 = getRawColor(strips, 3, invert);
                    
    dst.pattern.data(xOffset + 0,
                 yOffset + 0)
                 = getRawColor(strips, 4, invert);
    dst.pattern.data(xOffset + 0,
                 yOffset + 1)
                 = getRawColor(strips, 5, invert);
    dst.pattern.data(xOffset + 0,
                 yOffset + 2)
                 = getRawColor(strips, 6, invert);
    dst.pattern.data(xOffset + 0,
                 yOffset + 3)
                 = getRawColor(strips, 7, invert);
  }
  
  int getRawColor(StripCollection& strips, int pos,
                  bool invert = false) {
    int raw;
    
/*    if (invert) {
      raw = 0x0F;
    }
    else {
      raw = 0x00;
    } */
    
    raw = 0x00;
    
    if (invert) {
      raw = 0x0F;
    }
    
    int mask = 0x08;
    for (int i = 0; i < strips.size(); i++) {
      ColorStrip& strip = strips[i];
      bool check = false;
      switch (pos) {
      case 0: check = strip.r0; break;
      case 1: check = strip.r1; break;
      case 2: check = strip.r2; break;
      case 3: check = strip.r3; break;
      case 4: check = strip.l0; break;
      case 5: check = strip.l1; break;
      case 6: check = strip.l2; break;
      case 7: check = strip.l3; break;
      default:
        
        break;
      }
      
      if (invert) {
        // if the strip is real, then override the current bit
        // with the value of check
        if (strip.isReal) {
          if (check) {
            // set bit (could skip this, since set is default)
            raw |= mask;
          }
          else {
            // clear bit
            raw = (raw & (mask ^ 0xFF));
          }
        }
        // if the strip is not real (i.e. inserted by the program
        // during the decompression process), leave the current
        // bit unchanged
        else {
          
        }
      }
      else {
        if (check) {
          raw ^= mask;
        }
      }
      
      mask >>= 1;
    }
    
    return raw;
  }
  
  TColor getColor(StripCollection& strips, int pos,
                  bool invert = false) {
    int raw = getRawColor(strips, pos, invert);
    
    return TColor(raw * 16, raw * 16, raw * 16,
                  (raw == 0) ? TColor::fullAlphaTransparency
                             : TColor::fullAlphaOpacity);
  }
};

int readCompressedGraphics(std::vector<MdPattern>& patterns,
                           unsigned char* src) {
  patterns.clear();
                           
  int pos = 0;

  int settings = src[pos++];
  int numTiles = src[pos++];
  
  // terminal marker?
  if (settings == 0xFF) {
    return 0;
  }
  // ???
  else if ((settings & 0x3F)) {
    return 0;
  }
  // uncompressed
  else if (!(settings & 0x80)) {
    patterns.resize(numTiles);
    
    for (int i = 0; i < numTiles; i++) {
      readPattern(patterns[i], (char*)src + pos);
      pos += bytesPerPattern;
    }
    
/*    for (int i = 0; i < numTiles; i++) {
      TGraphic& tile = dst[i];
      tile.resize(patternW, patternH);
    }
    for (int i = 0; i < numTiles; i++) {
      readPattern(dst[i], (char*)src + pos);
      pos += bytesPerPattern;
    } */
  
    return pos;
  }
//  else if (settings & 0x40) {
//    return 0;
//  }
  
  patterns.resize(numTiles);
  
  for (int i = 0; i < numTiles; i++) {
    MdPattern& pattern = patterns[i];
  
    std::vector<int> compressionBytes;
    compressionBytes.resize(4);
    
    for (int j = 0; j < compressionBytes.size(); j++) {
      compressionBytes[j] = src[pos++];
    }
    
    StripAggregate strips;
    
    for (int j = 0; j < compressionBytes.size(); j++) {
//      std::cout << std::hex << compressionBytes[j] << " ";
      for (int mask = 0x80; mask > 0; mask >>= 1) {
        
        ColorStrip strip;
        if ((compressionBytes[j] & mask) != 0) {
          strip = ColorStrip(src[pos++]);
/*          std::cout << strip.r0 << " "
                    << strip.r1 << " "
                    << strip.r2 << " "
                    << strip.r3 << " "
                    << strip.l0 << " "
                    << strip.l1 << " "
                    << strip.l2 << " "
                    << strip.l3 << std::endl; */
//          std::cout << std::hex << (int)(src[pos - 1]) << std::endl;
        }
        
        switch (mask) {
        case 0x01: case 0x10:
          if (j < 2) {
            strips.stripsU3.push_back(strip);
          }
          else {
            strips.stripsD3.push_back(strip);
          }
          break;
        case 0x02: case 0x20:
          if (j < 2) {
            strips.stripsU2.push_back(strip);
          }
          else {
            strips.stripsD2.push_back(strip);
          }
          break;
        case 0x04: case 0x40:
          if (j < 2) {
            strips.stripsU1.push_back(strip);
          }
          else {
            strips.stripsD1.push_back(strip);
          }
          break;
        case 0x08: case 0x80:
          if (j < 2) {
            strips.stripsU0.push_back(strip);
          }
          else {
            strips.stripsD0.push_back(strip);
          }
          break;
        default:
          
          break;
        }
        
      }
    }
//    std::cout << std::endl;
//    strips.toTile(tile, ((settings & 0x40) != 0));
    strips.toMdPattern(pattern, ((settings & 0x40) != 0));
  }
  
  return pos;
}

/*int readCompressedGraphics(std::vector<TGraphic>& dst, unsigned char* src) {
  int pos = 0;

  int settings = src[pos++];
  int numTiles = src[pos++];
  
  // terminal marker?
  if (settings == 0xFF) {
    return 0;
  }
  // ???
  else if ((settings & 0x3F)) {
    return 0;
  }
  // uncompressed
  else if (!(settings & 0x80)) {
    std::vector<MdPattern> patterns;
    dst.resize(numTiles);
    patterns.resize(numTiles);
    
    for (int i = 0; i < numTiles; i++) {
      readPattern(patterns[i], (char*)src + pos);
      patterns[i].toGrayscaleGraphic(dst[i]);
      pos += bytesPerPattern;
    }
  
    return pos;
  }
//  else if (settings & 0x40) {
//    return 0;
//  }
  
  std::vector<MdPattern> patterns;
  dst.resize(numTiles);
  patterns.resize(numTiles);
  
  for (int i = 0; i < numTiles; i++) {
    MdPattern& pattern = patterns[i];
    TGraphic& tile = dst[i];
    tile.resize(patternW, patternH);
  
    std::vector<int> compressionBytes;
    compressionBytes.resize(4);
    
    for (int j = 0; j < compressionBytes.size(); j++) {
      compressionBytes[j] = src[pos++];
    }
    
    StripAggregate strips;
    
    for (int j = 0; j < compressionBytes.size(); j++) {
//      std::cout << std::hex << compressionBytes[j] << " ";
      for (int mask = 0x80; mask > 0; mask >>= 1) {
        
        ColorStrip strip;
        if ((compressionBytes[j] & mask) != 0) {
          strip = ColorStrip(src[pos++]);
        }
        
        switch (mask) {
        case 0x01: case 0x10:
          if (j < 2) {
            strips.stripsU3.push_back(strip);
          }
          else {
            strips.stripsD3.push_back(strip);
          }
          break;
        case 0x02: case 0x20:
          if (j < 2) {
            strips.stripsU2.push_back(strip);
          }
          else {
            strips.stripsD2.push_back(strip);
          }
          break;
        case 0x04: case 0x40:
          if (j < 2) {
            strips.stripsU1.push_back(strip);
          }
          else {
            strips.stripsD1.push_back(strip);
          }
          break;
        case 0x08: case 0x80:
          if (j < 2) {
            strips.stripsU0.push_back(strip);
          }
          else {
            strips.stripsD0.push_back(strip);
          }
          break;
        default:
          
          break;
        }
        
      }
    }
//    std::cout << std::endl;
//    strips.toTile(tile, ((settings & 0x40) != 0));
    strips.toMdPattern(pattern, ((settings & 0x40) != 0));
    pattern.toGrayscaleGraphic(tile);
  }
  
  return pos;
} */

typedef std::vector<unsigned char> CompressionByteCollection;
typedef TTwoDArray<bool> BitCollection;

struct CompressedHalfCharacter {
  
  int compressionMask;
  std::vector<char> compressionData;
  
  int getFirstCompressionByte() {
    return (compressionMask & 0xFF00) >> 8;
  }
  
  int getSecondCompressionByte() {
    return (compressionMask & 0x00FF);
  }
  
};

void bitCollectionToGraphic(TGraphic& dst, BitCollection& src,
                            int xOffset, int yOffset) {
  for (int j = 0; j < src.h(); j++) {
    for (int i = 0; i < src.w(); i++) {
    
      TColor color;
      if (src.data(i, j)) {
        color = TColor(0, 0, 0, TColor::fullAlphaOpacity);
      }
      else {
        color = TColor(255, 255, 255, TColor::fullAlphaTransparency);
      }
      
      dst.setPixel(xOffset + i, yOffset + j,
                   color);
    }
  }
}

void graphicToBitCollection(BitCollection& dst, TGraphic& src,
                            int xOffset, int yOffset) {
  for (int j = 0; j < src.h(); j++) {
    for (int i = 0; i < src.w(); i++) {
    
      TColor color = src.getPixel(xOffset + i, yOffset + j);
      if (color.a() == TColor::fullAlphaOpacity) {
        dst.data(i, j) = true;
      }
      else {
        dst.data(i, j) = false;
      }
    }
  }
}

int readCompressedQuarterCharacter(BitCollection& dst, unsigned char* src,
                                   int row,
                                   unsigned char compressionByte) {
  int pos = 0;
  
//  std::cout << std::hex << (int)compressionByte << std::endl;
  
  for (int i = 0; i < 8; i++) {
    int maski = (0x01 << (7 - i));
  
    if (compressionByte & maski) {
      unsigned char next = src[pos++];
      
      for (int j = 0; j < 8; j++) {
      
        int maskj = (0x01 << (7 - j));
        if (next & maskj) {
          
          // don't flip the last row unless we're on the last line
          int limit = dst.h() - 1;
          if ((i + row) == dst.h() - 1) {
            limit = dst.h();
          }
              
          // flip all bits in column at and below current row
          for (int k = i + row; k < limit; k++) {
            if (dst.data(j, k)) {
              dst.data(j, k) = false;
            }
            else {
              dst.data(j, k) = true;
            }
          }
          
        }
        
      }
      
    }
  }
  
  return pos;
}

void compressionColumnFlip(BitCollection& dst, int row, int col) {
  int limit;
  if (row < 15) limit = dst.h() - 1;
  else limit = dst.h();
  
  for (int i = row; i < limit; i++) {
    dst.data(col, i) = !(dst.data(col, i));
  }
}

void writeCompressedHalfCharacter(CompressedHalfCharacter& dst,
                                  BitCollection& src) {
  dst.compressionMask = 0;
  dst.compressionData.clear();
                                  
  BitCollection wip(8, 16);
  wip.fill(false);
  
  // find starting row
  // better solution would have been to have a dummy row and avoid
  // this redundant code entirely, but too late
  int i;
  for (i = 0; i < 16; i++) {
    bool done = false;
    
    char compressionByte = 0;
    for (int j = 0; j < 8; j++) {
      int maskj = (0x01 << (7 - j));
      if (src.data(j, i) == true) {
        compressionColumnFlip(wip, i, j);
        compressionByte |= maskj;
        done = true;
      }
    }
    
    if (done) {
      dst.compressionMask |= (0x01 << (15 - i));
      dst.compressionData.push_back(compressionByte);
      break;
    }
  }
  
//  std::cerr << i << std::endl;
//  char c; std::cin >> c;
  
  // initialize starting row
//  for (int j = 0; j < 8; j++) {
//    if (src.data(j, i)) {
//      compressionColumnFlip(wip, i, j);
//    }
//  }
    
  ++i;
  
  // iterate over rows
  for ( ; i < 16; i++) {
    int maski = (0x01 << (15 - i));
  
    // do we need to flip any columns on this row?
    char compressionByte = 0;
    for (int j = 0; j < 8; j++) {
      int maskj = (0x01 << (7 - j));
      
      // need flip if previous row of wip doesn't match current row of src) {
      if (i != 15) {
        if (src.data(j, i) != wip.data(j, i - 1)) {
          compressionColumnFlip(wip, i, j);
          compressionByte |= maskj;
        }
      }
      // handle last row specially
      else {
        if (src.data(j, i)) {
          compressionColumnFlip(wip, i, j);
          compressionByte |= maskj;
        }
      }
    }
    
    if (compressionByte != 0) {
      // update compression data
      dst.compressionMask |= maski;
      dst.compressionData.push_back(compressionByte);
    }
    
    
/*    for (int j = 0; j < 8; j++) {
    
      int maskj = (0x01 << (7 - j));
      if (next & maskj) {
        
        // don't flip the last row unless we're on the last line
        int limit = dst.h() - 1;
        if ((i + row) == dst.h() - 1) {
          limit = dst.h();
        }
            
        // flip all bits in column at and below current row
        for (int k = i + row; k < limit; k++) {
          if (dst.data(j, k)) {
            dst.data(j, k) = false;
          }
          else {
            dst.data(j, k) = true;
          }
        }
        
      }
        
    } */
  }
}

int writeCompressedCharacter(unsigned char* dst, BitCollection& src) {
  int pos = 0;
  
  // separate into two half-characters
  BitCollection left(8, 16);
  BitCollection right(8, 16);
  for (int j = 0; j < 16; j++) {
    for (int i = 0; i < 8; i++) {
      left.data(i, j) = src.data(i, j);
      right.data(i, j) = src.data(i + 8, j);
    }
  }
  
  // compress each character
  CompressedHalfCharacter first;
  CompressedHalfCharacter second;
  writeCompressedHalfCharacter(first, left);
  writeCompressedHalfCharacter(second, right);
  
  // write compression bytes
  dst[pos++] = first.getFirstCompressionByte();
  dst[pos++] = first.getSecondCompressionByte();
  dst[pos++] = second.getFirstCompressionByte();
  dst[pos++] = second.getSecondCompressionByte();
  
  // write compression data
  for (int i = 0; i < first.compressionData.size(); i++) {
    dst[pos++] = first.compressionData[i];
  }
  for (int i = 0; i < second.compressionData.size(); i++) {
    dst[pos++] = second.compressionData[i];
  }
  
  return pos;
}

int writeCompressedFont(unsigned char* dst, std::vector<TGraphic>& src) {
  
  int indexSize = src.size() * 2;
  int pos = indexSize;
  for (int i = 0; i < src.size(); i++) {
    BitCollection bits(16, 16);
    graphicToBitCollection(bits, src[i], 0, 0);
    // write index entry
    ByteConversion::toBytes(pos, dst + (i * 2), 2,
                            EndiannessTypes::big,
                            SignednessTypes::nosign);
    pos += writeCompressedCharacter((unsigned char*)dst + pos, bits);
  }
  
  return pos;
  
/*  int indexEnd = ByteConversion::fromBytes(src,
                                           ByteSizes::uint16Size,
                                           EndiannessTypes::big,
                                           SignednessTypes::nosign);
  
  int numEntries = indexEnd / ByteSizes::uint16Size;
  
  dst.resize(numEntries);
  
  int lastPos = indexEnd;
  for (int i = 0; i < numEntries; i++) {
//    dst[i].resize(16, 16);
  
    int offset = ByteConversion::fromBytes(src + (i * ByteSizes::uint16Size),
                                           ByteSizes::uint16Size,
                                           EndiannessTypes::big,
                                           SignednessTypes::nosign);
    
    lastPos = offset + readCompressedCharacter(dst[i], src + offset);
  }
  
  return lastPos; */
}

int readCompressedHalfCharacter(BitCollection& dst, unsigned char* src,
                                CompressionByteCollection compressionBytes) {
  int pos = 0;
  
  pos += readCompressedQuarterCharacter(dst, src + pos,
                                        0,
                                        compressionBytes[0]);
  
  pos += readCompressedQuarterCharacter(dst, src + pos,
                                        8,
                                        compressionBytes[1]);
                                        
  
  return pos;
}

int readCompressedCharacter(TGraphic& dst, unsigned char* src) {
  int pos = 0;
  
  dst.resize(16, 16);

  CompressionByteCollection compressionBytes1;
  CompressionByteCollection compressionBytes2;
  
  for (int i = 0; i < 2; i++) {
    compressionBytes1.push_back(src[pos++]);
  }
  
  for (int i = 0; i < 2; i++) {
    compressionBytes2.push_back(src[pos++]);
  }
  
  BitCollection bits1(8, 16);
  bits1.fill(false);
  BitCollection bits2(8, 16);
  bits2.fill(false);
  
  pos += readCompressedHalfCharacter(bits1, src + pos, compressionBytes1);
//  std::cout << pos << std::endl;
  pos += readCompressedHalfCharacter(bits2, src + pos, compressionBytes2);
  
  bitCollectionToGraphic(dst, bits1, 0, 0);
  bitCollectionToGraphic(dst, bits2, 8, 0);
  
  return pos;
}

int readCompressedFont(std::vector<TGraphic>& dst, unsigned char* src) {
  
  int indexEnd = ByteConversion::fromBytes(src,
                                           ByteSizes::uint16Size,
                                           EndiannessTypes::big,
                                           SignednessTypes::nosign);
  
  int numEntries = indexEnd / ByteSizes::uint16Size;
  
  dst.resize(numEntries);
  
  int lastPos = indexEnd;
  for (int i = 0; i < numEntries; i++) {
//    dst[i].resize(16, 16);
  
    int offset = ByteConversion::fromBytes(src + (i * ByteSizes::uint16Size),
                                           ByteSizes::uint16Size,
                                           EndiannessTypes::big,
                                           SignednessTypes::nosign);
    
    lastPos = offset + readCompressedCharacter(dst[i], src + offset);
  }
  
  return lastPos;
}

const static int oneBppCharSize = 0x20;

int read1bppChar(TGraphic& g, unsigned char* src) {
  
  for (int i = 0; i < oneBppCharSize; i += 2) {
    unsigned char first = src[i];
    unsigned char second = src[i + 1];
    
    for (int j = 0; j < 8; j++) {
      int mask = 0x01 << j;
      g.setPixel((7 - j) + 8, i / 2,
                 (second & mask)
                   ? TColor(0, 0, 0, TColor::fullAlphaOpacity)
                   : TColor(255, 255, 255, TColor::fullAlphaTransparency));
      g.setPixel((7 - j) + 0, i / 2,
                 (first & mask)
                   ? TColor(0, 0, 0, TColor::fullAlphaOpacity)
                   : TColor(255, 255, 255, TColor::fullAlphaTransparency));
    }
  }

  return oneBppCharSize;
}

const static int oneBppCharUSSize = 0x10;

int read1bppCharUS(TGraphic& g, unsigned char* src) {
  
  for (int i = 0; i < oneBppCharSize; i++) {
    unsigned char first = src[i];
    
    for (int j = 0; j < 8; j++) {
      int mask = 0x01 << j;
      g.setPixel((7 - j) + 0, i,
                 (first & mask)
                   ? TColor(0, 0, 0, TColor::fullAlphaOpacity)
                   : TColor(255, 255, 255, TColor::fullAlphaTransparency));
    }
  }
  
  return oneBppCharUSSize;
}

int write1bppCharUS(TGraphic& g, int x, int y, unsigned char* dst) {
  
  for (int i = y; i < y + (patternH * 2); i++) {
    unsigned char first = 0;
    for (int j = 0; j < 8; j++) {
//      std::cerr << i << " " << j << std::endl;
      int mask = 0x01 << j;
      TColor color = g.getPixel((7 - j) + x, i);
      if (color.a() == TColor::fullAlphaOpacity) {
        first |= mask;
      }
    }
    
    dst[i] = first;
  }
  
  return oneBppCharUSSize;
}

const static int portraitW = 6;
const static int portraitH = 6;
const static int numPortraitPatterns = portraitW * portraitH;
const static int totalPortraitPatternDataSize
  = (numPortraitPatterns * MdPattern::uncompressedSize);
const static int totalPortraitDataSize
  = MdPaletteLine::uncompressedSize + totalPortraitPatternDataSize;

void readPortrait(TGraphic& dst, TByte* src, MdPaletteLine line) {
  // patterns
  std::vector<MdPattern> patterns;
  for (int i = 0; i < numPortraitPatterns; i++) {
    MdPattern pattern;
    readPattern(pattern, (char*)src);
    src += MdPattern::uncompressedSize;
    patterns.push_back(pattern);
  }
  
  dst.resize(portraitW * MdPattern::w,
             portraitH * MdPattern::h);
  
  int patternNum = 0;
  for (int j = 0; j < portraitH; j++) {
    for (int i = 0; i < portraitW; i++) {
      TGraphic patGraphic;
      patterns[patternNum].toColorGraphic(patGraphic, line);
      dst.copy(patGraphic, TRect(i * MdPattern::w,
                                 j * MdPattern::h,
                                 0, 0));
      
      ++patternNum;
    }
  }
}

void readPortrait(TGraphic& dst, TByte* src) {
  // palette
  MdPaletteLine line(src);
  src += MdPaletteLine::uncompressedSize;
  
  readPortrait(dst, src, line);
}

void makeUniformMappedGraphic(TGraphic& dst, std::vector<MdPattern>& patterns,
                              const MdPaletteLine& line,
                              int w, int h) {
  dst.resize(w * MdPattern::w,
             h * MdPattern::h);
  int patternNum = 0;
  // note that patterns must be in column/row order!
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      TGraphic patGraphic;
      patterns[patternNum].toColorGraphic(patGraphic, line);
      dst.copy(patGraphic, TRect(i * MdPattern::w,
                                 j * MdPattern::h,
                                 0, 0));
      
      ++patternNum;
    }
  }
}

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    return 0;
  }
  
  int start = 0;
  if (argc >= 3) {
    start = TStringConversion::stringToInt(
                std::string(argv[2]));
  }
  
  std::ifstream ifs(argv[1], std::ios_base::binary);
  int fullsz = fsize(ifs);
  
  int end = fullsz;
  if (argc >= 4) {
    end = TStringConversion::fromString<int>(
                std::string(argv[3]));
  }
  int contentsz = end - start;
  
  int sz = fullsz - start;
  int w = rowWidth;
//  int h = ((fullsz / bytesPerPattern / patternsPerRow) + 1) * rowHeight;
  int h = ((contentsz / bytesPerPattern / patternsPerRow) + 1) * rowHeight;
  TGraphic grp(w, h);
  
  char* buffer = new char[fullsz];
//  ifs.seekg(start);
  ifs.read(buffer, fullsz);
  
  // raw content dump
/*  int i = start;
  while ((i + bytesPerPattern) <= end) {
    
    TGraphic pat(patternW, patternH);
    readPattern(pat, buffer + i);
    grp.copy(pat,
             TRect(((i - start) / bytesPerPattern % patternsPerRow)
                     * patternW,
                   ((i - start) / bytesPerPattern / patternsPerRow)
                     * patternH,
                   0, 0));
    
    i += bytesPerPattern;
  } */
  
  // raw 1bpp dump
/*  grp = TGraphic(w * 2, h * 2);
  int i = start;
  while ((i + oneBppCharSize) <= end) {
    
    TGraphic pat(patternW * 2, patternH * 2);
    read1bppChar(pat, (unsigned char*)(buffer + i));
    grp.copy(pat,
             TRect(((i - start) / bytesPerPattern % patternsPerRow)
                     * (patternW * 2),
                   ((i - start) / bytesPerPattern / patternsPerRow)
                     * (patternH * 2),
                   0, 0));
    
    i += oneBppCharSize;
  } */
  
  // raw 1bpp dump TSS US font
/*  grp = TGraphic(w * 2, h * 2);
  int i = start;
  while ((i + oneBppCharUSSize) <= end) {
    
    TGraphic pat(patternW * 1, patternH * 2);
    read1bppCharUS(pat, (unsigned char*)(buffer + i));
    grp.copy(pat,
             TRect(((i - start) / (bytesPerPattern / 2) % (patternsPerRow * 2))
                     * (patternW * 1),
                   ((i - start) / bytesPerPattern / patternsPerRow)
                     * (patternH * 2),
                   0, 0));
    
    i += oneBppCharUSSize;
  } */
  
  // write 1bpp TSS US font
/*  TGraphic srcgrp;
  TPngConversion::RGBAPngToGraphic("newfont.png", srcgrp);
  char* dst = buffer + start;
  for (int j = 0; j < srcgrp.h() / (patternH * 2) - 0; j++) {
    
    for (int i = 0; i < (srcgrp.w() / patternW) - 1; i++) {
//      std::cerr << srcgrp.w() << " " << srcgrp.h() << std::endl;
//      std::cerr << i << std::endl;
//      std::cerr << j << std::endl;
      dst += write1bppCharUS(srcgrp,
                i * (patternW * 1),
                j * (patternH * 2),
                (unsigned char*)dst);
    }
  }
  std::ofstream ofs(std::string(argv[1]) + "_font",
                    std::ios_base::binary);
  ofs.write(buffer, fullsz);  */
  
  // palettes
  // CMB00 effects
//  int paladdr = 0x1AB4E;
//  int limit = 0x1BBAE;
  // portrait0
/*  int paladdr = 0x288a;
  int limit = 0x28aa;
  std::vector<MdPaletteLine> lines;
  for (int i = paladdr; i < limit; i += MdPaletteLine::uncompressedSize) {
    MdPaletteLine line((TByte*)buffer + i);
    TGraphic graphic;
    line.generatePreview(graphic);
    TPngConversion::graphicToRGBAPng(std::string("palette")
                                         + "-"
                                         + TStringConversion::toString(i)
                                         + ".png",
                                       graphic);
    lines.push_back(line);
  } */
  
//  MdPaletteLine line((TByte*)buffer + i);
/*  MdPaletteLine line(standardPaletteLineDataSample);
  for (int i = 0xBB38; i < 0xF5B8; i += totalPortraitPatternDataSize) {
    TGraphic graphic;
    readPortrait(graphic, (TByte*)buffer + i, line);
    TPngConversion::graphicToRGBAPng(std::string("portrait")
                                         + "-"
                                         + TStringConversion::toString(
                                              (i - 0xBB38)
                                                / totalPortraitPatternDataSize)
                                         + ".png",
                                       graphic);
  } */
  
/*  for (int i = 0x288A; i < 0xA56A; i += totalPortraitDataSize) {
    TGraphic graphic;
    readPortrait(graphic, (TByte*)buffer + i);
    TPngConversion::graphicToRGBAPng(std::string("portrait")
                                         + "-"
                                         + TStringConversion::toString(
                                              (i - 0x288A)
                                                / totalPortraitDataSize)
                                         + ".png",
                                       graphic);
  } */
  
  // compressed graphics
  int addr = start;
//  int addr = 0xF4B0;
  int offset = 0;
  std::vector<TGraphic> test;
  std::vector<MdPattern> patterns;
  std::vector<MdPattern> allPatterns;
  while ((offset = readCompressedGraphics(
                      patterns, (unsigned char*)buffer + addr)) != 0) {
    
    // REMOVED STUFF GOES HERE
    for (int i = 0; i < patterns.size(); i++) {
      TGraphic graphic;
      patterns[i].toGrayscaleGraphic(graphic);
//      patterns[i].toColorGraphic(graphic, StandardPalette);
      TPngConversion::graphicToRGBAPng(TStringConversion::toString(addr)
                                         + "-"
                                         + TStringConversion::toString(i)
                                         + ".png",
                                       graphic);
    } 
    
    for (int i = 0; i < patterns.size(); i++) {
      allPatterns.push_back(patterns[i]);
    } 
    
    addr += offset;
  }
  std::cout << "stopped at " << addr << std::endl; 
  
  // REMOVED STUFF
/*  // standard overworld sprites
    if ((patterns.size() % 6) == 0) {
      for (int i = 0; i < patterns.size(); i += 6) {
        std::vector<MdPattern> subPats;
        for (int j = i; j < i + 6; j++) {
          subPats.push_back(patterns[j]);
        }
        
        TGraphic graphic;
        makeUniformMappedGraphic(graphic, subPats, StandardPalette,
                                 2, 3);
        TPngConversion::graphicToRGBAPng(TStringConversion::toString(addr)
                                           + "-"
                                           + TStringConversion::toString(i)
                                           + ".png",
                                         graphic);
      } 
    } */
  
/*  for (int i = 0; i < allPatterns.size() - 4; i += 4) {
    TGraphic output(16, 16);
    TGraphic ulG, urG, llG, lrG;
    allPatterns[i + 0].toColorGraphic(ulG, StandardPalette);
    allPatterns[i + 1].toColorGraphic(llG, StandardPalette);
    allPatterns[i + 2].toColorGraphic(urG, StandardPalette);
    allPatterns[i + 3].toColorGraphic(lrG, StandardPalette);
    output.copy(ulG, TRect(0, 0, 0, 0));
    output.copy(llG, TRect(0, 8, 0, 0));
    output.copy(urG, TRect(8, 0, 0, 0));
    output.copy(lrG, TRect(8, 8, 0, 0));
    int outnum = i / 4;
    if (outnum >= 52) {
      outnum += 5;
    }
    TPngConversion::graphicToRGBAPng(std::string("item-")
//                                       + TStringConversion::toString(addr)
//                                       + "-"
                                       + TStringConversion::toString(outnum)
                                       + ".png",
                                     output);
  } */
  
  // font
/*//  int addr = 0x2FB18;
  int addr = 0x2044;
  std::vector<TGraphic> font;
  int result = readCompressedFont(font, (unsigned char*)buffer + addr);
//  TGraphic outputFont(16 * 16, 16 * 8);
  for (int i = 0; i < font.size(); i++) {
//    outputFont.copy(font[i],
//                    TRect((i * 16) % outputFont.w(),
//                          (i / 16 * 16),
//                          0,
//                          0));
    TPngConversion::graphicToRGBAPng(TStringConversion::toString(addr)
                                       + "-"
                                       + TStringConversion::toString(i)
                                       + ".png",
                                     font[i]); 
  } 
//  TPngConversion::graphicToRGBAPng("lunareb_font.png",
//                                   outputFont);
  std::cout << "stopped at " << addr + result << std::endl; */
  
  // compress font
/*  std::vector<TGraphic> font2;
  TGraphic inputFont;
  TPngConversion::RGBAPngToGraphic("lunareb_newfont.png",
                                   inputFont);
  for (int i = 0; i < 128; i++) {
    int x = (i * 16) % (16 * 16);
    int y = (i / 16 * 16);
    TGraphic c(16, 16);
    c.copy(inputFont, TRect(0, 0, 0, 0), TRect(x, y, 0, 0));
    font2.push_back(c);
  }
  
  TByte test[0x20000];
  int result2 = writeCompressedFont(test, font2);
 
  std::ofstream ofs("lunareb_newfont_compressed.bin", std::ios_base::binary);
  ofs.write((char*)test, result2); */
  
  delete buffer;
  
  TPngConversion::graphicToRGBAPng(std::string(argv[1])
                                     + ".png",
                                   grp); 
  
  return 0;
}
