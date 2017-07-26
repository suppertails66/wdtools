#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TStringConversion.h"
#include "util/ByteConversion.h"
#include "util/TTwoDArray.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace BlackT;

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
  ifstream ifs(filename, ios_base::binary);
  int sz = fsize(ifs);
  
  BufferedFile result;
  result.buffer = new char[sz];
  result.size = sz;
  ifs.read(result.buffer, sz);
  
  return result;
}

inline TColor pixelToColor4bpp(int value) {
  int level = (int)(value << 4) | value;
  return TColor(level, level, level,
                (value == 0) ? TColor::fullAlphaTransparency
                  : TColor::fullAlphaOpacity);
}

void placePixel(TGraphic& dst, TColor color, int pixelNum) {
  int x = (pixelNum % dst.w());
  int y = (pixelNum / dst.w());
  dst.setPixel(x, y, color);
}

void read4bppGraphicGrayscale(const unsigned char* src,
                              TGraphic& dst,
                              int width,
                              int height) {
  dst.resize(width, height);
  
  int numBytes = (width * height) / 2;
  
  for (int i = 0; i < numBytes; i++) {
    int pix1 = (*src & 0xF0) >> 4;
    int pix2 = (*src & 0x0F);
    
    TColor color1 = pixelToColor4bpp(pix1);
    TColor color2 = pixelToColor4bpp(pix2);
    
    placePixel(dst, color1, (i * 2));
    placePixel(dst, color2, (i * 2) + 1);
    
    src++;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    return 0;
  }
  
  BufferedFile indexFile = getFile(argv[1]);
  BufferedFile imgFile = getFile(argv[2]);
  
  for (int i = 0; i < 0x18; i++) {
    int indexPos = (i * 0x18) + 0x6c;
    int imgPos =
      (ByteConversion::fromBytes(indexFile.buffer + indexPos + 4, 2,
                                 EndiannessTypes::big,
                                 SignednessTypes::nosign)
        << 3);
    int width =
      (ByteConversion::fromBytes(indexFile.buffer + indexPos + 20, 2,
                                 EndiannessTypes::big,
                                 SignednessTypes::nosign)
        << 1) + 2;
    int height =
      (ByteConversion::fromBytes(indexFile.buffer + indexPos + 18, 2,
                                 EndiannessTypes::big,
                                 SignednessTypes::nosign)
        << 1) + 1;
    
//    cout << imgPos << " " << width << " " << height << endl;

    TGraphic g(width, height);
    read4bppGraphicGrayscale((unsigned char*)(imgFile.buffer) + imgPos,
                             g, width, height);
    TPngConversion::graphicToRGBAPng(
      TStringConversion::toString(i) + ".png", g);
  }
  
  delete indexFile.buffer;
  delete imgFile.buffer;
  
  return 0; 
}
