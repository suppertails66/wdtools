#include "mgl_cmpr.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace BlackT;

const int decompressionBufferSize = 0x100000;

const int fieldbodPaletteOffset = 0x0;
const int fieldbodPaletteSize = 0x200;
const int fieldbodPaletteEntrySize = 0x2;

const int indexOffset = 0x200;
const int indexSize = 0x200;
const int indexStructSize = 0xC;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Mahou Gakuen Lunar! FIELD.BOD potrait extractor" << endl; 
    cout << "Usage: " << argv[0] << " <infile>" << endl;
    return 0;
  }
  
  BufferedFile file = getFile(argv[1]);
  char* fileP = file.buffer;
  
  vector<TColor> palette;
  for (int i = 0; i < (fieldbodPaletteSize / fieldbodPaletteEntrySize); i++) {
    int pos = fieldbodPaletteOffset + (i * fieldbodPaletteEntrySize);
    int rawColor = ByteConversion::fromBytes(fileP + pos, 2,
      EndiannessTypes::big, SignednessTypes::nosign);
    int b = (rawColor & 0x7C00) >> 7;
    int g = (rawColor & 0x03E0) >> 2;
    int r = (rawColor & 0x001F) << 3;
    palette.push_back(TColor(r, g, b,
      (i == 0) ? TColor::fullAlphaTransparency : TColor::fullAlphaOpacity));
  }
  
  int numIndexEntries = ByteConversion::fromBytes(fileP + indexOffset, 4,
      EndiannessTypes::big, SignednessTypes::nosign);
  for (int i = 0; i < numIndexEntries; i++) {
    int pos = indexOffset + (i * indexStructSize) + 4;
    
    int w = ByteConversion::fromBytes(fileP + pos + 0, 2,
      EndiannessTypes::big, SignednessTypes::nosign);
    int h = ByteConversion::fromBytes(fileP + pos + 2, 2,
      EndiannessTypes::big, SignednessTypes::nosign);
    int offset = ByteConversion::fromBytes(fileP + pos + 4, 4,
      EndiannessTypes::big, SignednessTypes::nosign);
    int size = ByteConversion::fromBytes(fileP + pos + 8, 4,
      EndiannessTypes::big, SignednessTypes::nosign)
        + 4;
      
//    int size = ByteConversion::fromBytes(fileP + offset, 4,
//      EndiannessTypes::big, SignednessTypes::nosign);
      
    cout << pos << " " << " " << w << " " << h
         << " " << offset << " " << size << endl;
    
    unsigned char decompressionBuffer[decompressionBufferSize];
    std::memset((char*)decompressionBuffer, 0, decompressionBufferSize);
    decompressMglImg((unsigned char*)fileP + offset,
                     size,
                     decompressionBuffer);
    
    TGraphic g;
    
//    read8bppGraphicGrayscale(decompressionBuffer, g, w, h);
    read8bppGraphicPalettized(decompressionBuffer, g, w, h, palette);
    
    TPngConversion::graphicToRGBAPng(TStringConversion::toString(i) + ".png",
                                     g);
  }
  
  // Clean up
  delete file.buffer;
  
  return 0;
}
