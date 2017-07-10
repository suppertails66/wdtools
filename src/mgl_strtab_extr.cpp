#include "mgl_transtxt.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"
#include "util/ByteConversion.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;
using namespace BlackT;

void readMglString(istream& ifs, string& dst) {
  unsigned int next = (unsigned char)(ifs.get());
  
  while (next != 0) {
    dst += (char)next;
    next = (unsigned char)(ifs.get());
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Usage: " << argv[0] << "formatfile" << endl;
    return 0;
  }
  
  ifstream fmtifs(argv[1], ios_base::binary);
  ifstream srcifs;
  string openedSrcFile;
  unsigned int openedSrcFileSize = 0;
  
  while (true) {
    string filename;
    while (fmtifs.good()) {
      fmtifs >> filename;
      
      // skip comments
      if (filename.size() && (filename[0] == ';')) {
        getline(fmtifs, filename);
      }
      else {
        break;
      }
    }
  
    // check for EOF
    if (!fmtifs.good()) break;
    fmtifs.get();
    if (!fmtifs.good()) break;
    fmtifs.unget();
  
    // get target info
    
//    cerr << hex << fmtifs.tellg() << endl;
    
    string loadaddrS, startaddrS, endaddrS;
    fmtifs >> loadaddrS >> startaddrS >> endaddrS;
    
//    cerr << filename << " " << loadaddrS << " " << startaddrS << " " << endaddrS << endl;
    
    // throw away rest of line
    string garbage;
    getline(fmtifs, garbage);
    
    unsigned int loadaddr = TStringConversion::stringToInt(loadaddrS);
    unsigned int startaddr = TStringConversion::stringToInt(startaddrS);
    unsigned int endaddr = TStringConversion::stringToInt(endaddrS);
    
    // open target file if not already open
    if (openedSrcFile.compare(filename) != 0) {
      srcifs.close();
      srcifs.open(filename, ios_base::binary);
      openedSrcFile = filename;
      openedSrcFileSize = fsize(srcifs);
    }
    
    unsigned int tableaddr = startaddr;
    unsigned int tablesize = endaddr - startaddr;
    unsigned int numentries = tablesize / 4;
    
//    cout << hex << tableaddr << " " << tablesize << " " << numentries << endl;
//    char c; cin >> c;
    
    srcifs.seekg(tableaddr);
    char* strtab = new char[tablesize];
    srcifs.read(strtab, tablesize);
    for (unsigned int i = 0; i < numentries; i++) {
      // read string pointer
      unsigned int strpointer = ByteConversion::fromBytes(
        strtab + (i * 4), 4, EndiannessTypes::big, SignednessTypes::nosign);
//      cout << hex << strpointer << " " << loadaddr << endl;

      // ignore null pointers
      if (strpointer == 0) continue;
      
      // convert to address within file
      unsigned int realpos = strpointer - loadaddr;

      if (realpos >= openedSrcFileSize) {
//        cout << "bad pointer!" << endl;
        continue;
      }
      // 0x80000000+ is well outside the SH-2's memory map
      if ((signed int)realpos < 0) continue;
      
//      cout << realpos << endl;
      
      // read string
      srcifs.seekg(realpos);
      string str;
      readMglString(srcifs, str);
      string escapedStr;
      escapeString(str, escapedStr);
//      string escapedStr = str;
      
      TranslationEntry e;
      e.sourceFile = filename;
      e.sourceFileOffset = realpos;
      e.originalText = escapedStr;
      e.originalSize = str.size();
      e.translatedText = "X";
      e.pointers.push_back(tableaddr + (i * 4));
      e.save(cout);
//      cout << setw(6) << hex << realpos << " " << str << endl;
    }
    delete strtab;
    
  }
  
  
  return 0;
}
