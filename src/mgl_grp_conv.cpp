#include "mgl_cmpr.h"
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
        
    int cmdsize =
      ByteConversion::fromBytes(indexFile.buffer + indexPos + 6, 2,
                                 EndiannessTypes::big,
                                 SignednessTypes::nosign);
    int width = ((cmdsize & 0x3F00) >> 8) << 3;
    int height = (cmdsize & 0xFF);
                                 
/*    int width =
      (ByteConversion::fromBytes(indexFile.buffer + indexPos + 20, 2,
                                 EndiannessTypes::big,
                                 SignednessTypes::nosign)
        << 1) + 2;
    int height =
      (ByteConversion::fromBytes(indexFile.buffer + indexPos + 18, 2,
                                 EndiannessTypes::big,
                                 SignednessTypes::nosign)
        << 1) + 1; */
    
//    cout << imgPos << " " << width << " " << height << endl;

    TGraphic g(width, height);
    read4bppGraphicGrayscale((unsigned char*)(imgFile.buffer) + imgPos,
                             g, width, height);
    
//    unsigned char buffer[0x100000];
//    writeGraphic4bpp(g, buffer);
//    TGraphic g2;
//    read4bppGraphicGrayscale(buffer, g2, width, height);
    
    TPngConversion::graphicToRGBAPng(
      TStringConversion::toString(i) + ".png", g);
  }
  
  delete indexFile.buffer;
  delete imgFile.buffer;
  
  return 0; 
}
