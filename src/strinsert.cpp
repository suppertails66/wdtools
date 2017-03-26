#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cctype>
#include "util/TByte.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"

using namespace std;
using namespace BlackT;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

struct CharEntry {
  
  enum Type {
    typeNone,
    typeLiteral,
    typeSymbol
  };
  
  Type type;
  TByte literal;
  string symbol;
  
  void load(istream& ifs) {
    type = typeNone;
    literal = 0;
    symbol = "";
  
    char next = ifs.get();
    // escaped literals
    if (next == '\\') {
      type = typeLiteral;
      literal = ifs.get();
    }
    // symbol
    else if (next == '[') {
      type = typeSymbol;
      next = ifs.get();
      
      // read symbol name
      while (next != ']') {
        symbol += next;
        next = ifs.get();
      }
      
      // check if this is a portrait attribution
      if ((symbol.size() == 5)
          && ((symbol.substr(0, 3).compare("POR") == 0)
              || (symbol.substr(0, 3).compare("ROR") == 0))) {
        string num = symbol.substr(3, 2);
        symbol = symbol.substr(0, 3);
        literal = TStringConversion::stringToInt(num);
//        cerr << num << " " << (int)literal << endl;
        return;
      }
      
      // check if this is actually a number literal
      int numcheck = TStringConversion::stringToInt(symbol);
      if ((symbol.compare("0") == 0)
          || (numcheck != 0)) {
        type = typeLiteral;
        literal = numcheck;
      }
    }
    else if (next == '\n') {
      char nextnext = ifs.peek();
      
      // dialogue break
/*      if (nextnext == '\n') {
        ifs.get();
        type = typeLiteral;
        literal = 0x02;
      }
      // real newline
      else { */
        type = typeLiteral;
        literal = 0x01;
//      }
    }
    // literal
    else {
      type = typeLiteral;
      literal = next;
    }
  }
  
};

struct StringEntry {
  
  int address;
  int origLength;
  string data;
  
};

struct StringEntries {
  
  typedef vector<StringEntry> StringEntryCollection;
  
  void load(istream& ifs) {
    stringEntries.clear();
    
    string nextstr;
    // "File: "
    ifs >> nextstr;
    while (isspace(ifs.peek())) {
      ifs.get();
    }
    string filename;
    getline(ifs, filename);
    
    // "Messages: "
    ifs >> nextstr;
    while (isspace(ifs.peek())) {
      ifs.get();
    }
    string nummessages;
    getline(ifs, nummessages);
  
    while (ifs.good()) {
      addStringEntry(ifs);
    }
  }
  
  void addStringEntry(istream& ifs) {
  
    if (!ifs.good()) {
      return;
    }
  
    string nextstr;
    ifs >> nextstr;
    
    if (!ifs.good()) {
      return;
    }
    
    if (nextstr.compare("===") != 0) {
      return;
    }
    
    // skip message num
    ifs >> nextstr;
    
    // " | "
    ifs >> nextstr;
    
    StringEntry entry;
    
    ifs >> hex >> entry.address;
    
    // " | "
    ifs >> nextstr;
    
    int length;
    ifs >> hex >> length;
    entry.origLength = length;
    
    // trailing ===
    ifs >> nextstr;
    
    // trailing newline
    string garbage;
    getline(ifs, garbage);
//    while (ifs.peek() == '\n') {
//      ifs.get();
//    }
    
    bool done = false;
    while (!done) {
      CharEntry next;
      next.load(ifs);
      
      switch (next.type) {
      case CharEntry::typeSymbol:
        if (next.symbol.compare("END") == 0) {
          done = true;
          break;
        }
        // dialogue breaks
        else if (next.symbol.compare("BRK") == 0) {
          // skip newlines (added in the extracted text for readability)
          ifs.get();
          ifs.get();
          
          entry.data += 0x02;
        }
        // portrait attribution
        else if ((next.symbol.compare("POR") == 0)
                 || (next.symbol.compare("ROR") == 0)) {
          int num = next.literal;
          if (next.symbol.compare("POR") == 0) {
            entry.data += (char)0x0C;
          }
          else {
            entry.data += (char)0x0F;
          }
          entry.data += (char)0x00;
          entry.data += (char)num;
          // skip newline
          ifs.get();
        }
        break;
      case CharEntry::typeLiteral:
        entry.data += next.literal;
        // prevent overflow
//        if (entry.data.size() >= length) {
//          done = true;
//          break;
//        }
        break;
      default:
        
        break;
      }
    }
    
    // if we didn't fill up the full string length, pad with newlines
    while (entry.data.size() < length) {
      entry.data += (char)(0x01);
    }
    
//    cout << entry.data << endl;
    
    stringEntries.push_back(entry);
    
  }
  
  void patch(TByte* dst, int size) {
    for (int i = 0; i < stringEntries.size(); i++) {
      StringEntry& entry = stringEntries[i];
      
      if ((entry.address >= size)) {
        cerr << "Warning: skipping patch with invalid offset "
             << entry.address << " (size: "
             << size << ")" << endl;
        continue;
      }
      else if ((entry.address + entry.data.size() >= size)) {
        cerr << "Warning: skipping patch that overruns script: "
             << entry.address << "(patch: " << entry.data.size()
             << " size: "
             << size << ")" << endl;
        continue;
      }
      
      if (entry.data.size() > entry.origLength) {
        cerr << "Warning: script overrun: "
             << entry.address
             << " (orig " << entry.origLength
             << ", new " << entry.data.size()
             << ", content: "
             << entry.data
             << ")" << endl;
      }
      
      std::memcpy((char*)(dst + entry.address),
                  entry.data.c_str(),
                  entry.data.size());
    }
  }
  
  StringEntryCollection stringEntries;
  
};

int main(int argc, char* argv[]) {
  
  
  if (argc < 4) {
    return 0;
  }
  
  string strFile = string(argv[1]);
  string baseFile = string(argv[2]);
  string outFile = string(argv[3]);
  
  ifstream strifs(strFile);
  ifstream baseifs(baseFile, ios_base::binary);
  int fileSize = fsize(baseifs);
  TByte* buffer = new TByte[fileSize];
  baseifs.read((char*)buffer, fileSize);
  baseifs.close();
  
  StringEntries stringEntries;
  stringEntries.load(strifs);
  stringEntries.patch(buffer, fileSize);
  
  ofstream ofs(outFile.c_str(), ios_base::binary | ios_base::trunc);
  ofs.write((char*)buffer, fileSize);
  
  delete buffer;
  
  return 0;
}
