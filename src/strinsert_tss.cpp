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
      if ((symbol.size() == 6)
          && ((symbol.substr(0, 3).compare("POR") == 0)
              || (symbol.substr(0, 3).compare("ROR") == 0))) {
        string num = symbol.substr(3, 3);
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
//    int msgnum;
//    ifs >> msgnum;
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
      
//      cerr << next.type << endl;
//      cerr << next.symbol << endl;

//      cerr << ifs.tellg() << endl;
//      char c; cin >> c;
      
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
//          if (next.symbol.compare("POR") == 0) {
            entry.data += (char)0x04;
//          }
//          else {
//            entry.data += (char)0x0F;
//          }
//          entry.data += (char)0x00;
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
    
//    cerr << entry.data << endl;
    
    // if we didn't fill up the full string length, pad with newlines
/*    while (entry.data.size() < length) {
      entry.data += (char)(0x01);
    } */
    // the above no longer works due to the presence of a dialogue arrow
    // in TSS, so we instead have to pad with spaces.
    // If the messages spans at least two lines, we can shove all of these
    // at the end of the first line without consequence.
    string padding;
    if (entry.origLength > (int)(entry.data.size())) {
      for (int i = 0; i < entry.origLength - entry.data.size(); i++) {
        padding += ' ';
      }
      if (padding.size() != 0) {
        bool done = false;
        for (int i = 0; i < entry.data.size(); i++) {
          if (entry.data[i] == 0x01) {
          
            // screen for portraits and stuff
            if ((i != 0)
                && ((entry.data[i - 1] == 0x04)
                    || (entry.data[i - 1] == 0x09))) {
              continue;
            }
          
            // insert padding
            entry.data = entry.data.substr(0, i)
                         + padding
                         + entry.data.substr(i, entry.data.size() - i);
            done = true;
            break;
          }
        }
        
        if (!done) {
          cerr << "Warning: visible padding: "
               << entry.address
               << " (orig " << entry.origLength
               << ", new " << entry.data.size()
               << ", content: "
               << entry.data
               << ")" << endl;
            entry.data = entry.data.substr(0, entry.data.size() - 1)
                         + padding
                         + entry.data.substr(entry.data.size() - 1, 1);
        }
      }
      
  //    cout << entry.data << endl;
      
    }
//    else if (entry.origLength < (int)(entry.data.size())) {
//      cerr << entry.origLength << " " << (int)(entry.data.size()) << endl;
//    }
      
    stringEntries.push_back(entry);
  }
  
  void patch(TByte* dst, int size) {
    for (int i = 0; i < stringEntries.size(); i++) {
      StringEntry& entry = stringEntries[i];
//        cerr << entry.data << endl;
      
//      if (i == 14) {
//        cerr << entry.origLength << " " << entry.data.size();
//      }
      
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
    cout <<
    "strinsert: Lunar: The Silver Star (Sega CD) English dialogue inserter\n"
    "Usage: strinsert <strings> <base> <output>\n"
    "<strings> is a file containing output from scriptrip or scriptrip_jp.\n"
    "<base> is the map file to be patched.\n"
    "<output> is the name of the patched output file.\n";
    return 0;
  }
  
  string strFile = string(argv[1]);
  string baseFile = string(argv[2]);
  string outFile = string(argv[3]);
  
  ifstream strifs(strFile.c_str());
  ifstream baseifs(baseFile.c_str(), ios_base::binary);
  int fileSize = fsize(baseifs);
  TByte* buffer = new TByte[fileSize];
  baseifs.read((char*)buffer, fileSize);
  baseifs.close();
  
  StringEntries stringEntries;
  if (fsize(strifs) > 0) {
    stringEntries.load(strifs);
    stringEntries.patch(buffer, fileSize);
  }
  
  ofstream ofs(outFile.c_str(), ios_base::binary | ios_base::trunc);
  ofs.write((char*)buffer, fileSize);
  
  delete buffer;
  
  return 0;
}
