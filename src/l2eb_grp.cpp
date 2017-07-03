#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TSerialize.h"
#include "util/TStringConversion.h"
#include "util/ByteConversion.h"

using namespace std;
using namespace BlackT;

bool readPck(istream& ifs, TGraphic& dst, int first,
             int width) {
//  if (d != 0x0100) {
//    return false;
//  }

  int w = width;
  int h = 256;
  
//  int w = 64;
//  int h = 2096;

//  w = 432;
//  h = 2048;
  
  dst.resize(w, h);
  
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      unsigned char next = ifs.get();
      dst.setPixel(i, j, TColor(next, next, next, TColor::fullAlphaOpacity));
    }
  }
  
  return true;
}

int main(int argc, char* argv[]) {
  if (argc < 3) return 0;
  
  ifstream ifs(argv[1], ios_base::binary);
  
/*  TGraphic g;
  readPck(ifs, g, 0, 256);
  if (g.w() != 0) {
    string filename = string(argv[2]) + "-test" + ".png";
    TPngConversion::graphicToRGBAPng(filename, g);
  } */
  
  int first = TSerialize::readInt(
    ifs, 2, EndiannessTypes::little, SignednessTypes::nosign);
  int a = TSerialize::readInt(
    ifs, 2, EndiannessTypes::little, SignednessTypes::nosign);
  int width = TSerialize::readInt(
    ifs, 4, EndiannessTypes::little, SignednessTypes::nosign);
  cout << hex << "first: " << first << endl;
  cout << hex << "a: " << a << endl;
  cout << endl;
  
  vector<TGraphic> graphics;
  do {
//    first = 320;
    cout << hex << "Graphic " << graphics.size() << endl;
    cout << hex << "pos: " << ifs.tellg() << endl;
    cout << hex << "width: " << width << endl;
    if (width & 0xFF0000) {
      width >>= 8;
      graphics.push_back(TGraphic());
      bool result = readPck(ifs, graphics[graphics.size() - 1], first, width);
      cout << hex << "Standard: endpos: " << ifs.tellg() << endl << endl;
      if (!result) {
        cerr << "Bad read at " << hex << ifs.tellg() << endl;
        break;
      }
    }
    else {
      graphics.push_back(TGraphic());
      ifs.seekg((unsigned int)(ifs.tellg()) + width);
      cout << hex << "Skipped! endpos: " << ifs.tellg() << endl << endl;
    }
    
//    first = TSerialize::readInt(
//      ifs, 2, EndiannessTypes::big, SignednessTypes::nosign);
//    if ((first & 0xFF) != 0) {
//      cerr << "Bad first at " << hex << ifs.tellg() << endl;
//      break;
//    }
    
    width = TSerialize::readInt(
      ifs, 4, EndiannessTypes::little, SignednessTypes::nosign);
    
    if ((width <= 0) || (width >= 0x100000)) {
      cerr << "Bad width at " << hex << ifs.tellg() << endl;
      break;
    }
    
  } while ((ifs.good()));
  
  for (unsigned int i = 0; i < graphics.size(); i++) {
    if (graphics[i].w() == 0) continue;
    string filename = string(argv[2]) + "-"
      + TStringConversion::toString(i) + ".png";
//    ofstream ofs(filename.c_str(), ios_base::binary);
    TPngConversion::graphicToRGBAPng(filename, graphics[i]);
  }
  
  return 0;
}
