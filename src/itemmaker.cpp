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

const static int numItemIcons = 192;
const static int magicIconsStart = 193;
const static int numIcons = 243;
const static int iconW = 16;
const static int iconH = 16;
const static int rowSpacing = 8;
const static int yMargin = 8;
const static int xMargin = 8;
// label string, two icons + space between, space between
// label and icons
const static int outputW = ((19 * iconW) / 2) + (3 * iconW) + (1 * iconW)
                            + (xMargin * 2);
const static int outputH = (numItemIcons * iconH)
                            + yMargin
                            + (numItemIcons * rowSpacing);

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

struct Font {
  
  const static int numRawGlyphs = 128;
  const static int characterW = 8;
  const static int characterH = 16;
  
  Font() { }
  Font(string basename) {
    for (int i = 0; i < numRawGlyphs; i++) {
      string filename = basename + TStringConversion::toString(i) + ".png";
      readGlyph(filename);
    }
  }
  
  TGraphic& getAsciiCharacter(int index) {
    return characters[((int)(index)) - 0x20];
  }
  
  void readGlyph(string filename) {
    TGraphic glyph;
    TPngConversion::RGBAPngToGraphic(filename,
                                     glyph);
    TGraphic first(characterW, characterH);
    TGraphic second(characterW, characterH);
    
    first.copy(glyph,
               TRect(0, 0, 0, 0),
               TRect(0, 0, characterW, characterH));
    second.copy(glyph,
               TRect(0, 0, 0, 0),
               TRect(characterW, 0, characterW, characterH));
               
    characters.push_back(first);
    characters.push_back(second);
  }
  
  vector<TGraphic> characters;
  
};

struct StringTable {
  
  StringTable() { }
  StringTable(string filename) {
    ifstream ifs(filename.c_str());
    
    while (ifs.good()) {
      // number
      string ignore;
      ifs >> ignore;
      
      if (!ifs.good()) {
        break;
      }
      
      // name
      string name;
      getline(ifs, name, '[');
      
      // "END]"
      getline(ifs, ignore);
      
      strings.push_back(name);
    }
  }
  
  string getString(int index) {
    return strings[index];
  }
  
  vector<string> strings;
  
};

int main(int argc, char* argv[]) {
  
  Font font("font/8260-");
  StringTable itemStrings("items_q.txt");
  StringTable magicStrings("magicsreal_q.txt");
  
//  TPngConversion::graphicToRGBAPng("test.png",
//                                   font.getAsciiCharacter('R'));

  TGraphic output(outputW, outputH);
  output.clearTransparent();
  
  int ypos = yMargin;
  for (int i = magicIconsStart; i < numIcons; i++) {
    int xpos = xMargin;
    
    int samp_num = i;
    int final_num = i;
    
    // some icons are missing from sample version
//    if (i >= 58) {
//      samp_num -= 5;
//    }
//    else if (i >= 53) {
//      samp_num = 0;
//    }
  
    string base_filename_samp = "item-"
      + TStringConversion::toString(samp_num)
      + ".png";
    string base_filename_final = "item-"
      + TStringConversion::toString(final_num)
      + ".png";
    
    string samp_filename = "items_samp/"
      + base_filename_samp;
    string final_filename = "items/"
      + base_filename_final;
    
    ifstream ifs1(samp_filename);
    ifstream ifs2(final_filename);
    int sz = fsize(ifs1);
    if (sz <= 0) {
      continue;
    }
    sz = fsize(ifs2);
    if (sz <= 0) {
      continue;
    }
    
    ifs1.close();
    ifs2.close();
    
    TGraphic samp;
    TGraphic final;
    TPngConversion::RGBAPngToGraphic(samp_filename,
                                     samp);
    TPngConversion::RGBAPngToGraphic(final_filename,
                                     final);
    
    output.blit(samp, TRect(xpos, ypos, 0, 0));
    xpos += iconW;
    xpos += iconW;
    output.blit(final, TRect(xpos, ypos, 0, 0));
    xpos += iconW;
    xpos += iconW;
    
    // items
//    string itemName = itemStrings.getString(i - 1);
    // magic
    string itemName = magicStrings.getString(i - magicIconsStart);
    
    for (int j = 0; j < itemName.size(); j++) {
      TGraphic character = font.getAsciiCharacter(itemName[j]);
      output.blit(character, TRect(xpos, ypos, 0, 0));
      xpos += Font::characterW;
    }
    
    ypos += iconH + rowSpacing;
  }
  
  TGraphic finalOutput(outputW, ypos);
  finalOutput.copy(output,
                   TRect(0, 0, 0, 0),
                   TRect(0, 0, 0, 0));
  
  TPngConversion::graphicToRGBAPng("output.png",
                                   finalOutput);
  
  return 0;
}
