#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include "util/TByte.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"

using namespace std;
using namespace BlackT;
  
// If true, the program tries to heuristically guess proper case.
// Otherwise, it outputs the raw text.
const static bool changeCase = true;

// If changeCase is on, capitalize after punctuation marks.
const static bool punctCaps = true;

// If changeCase is on, capitalize after spaces.
const static bool spaceCaps = false;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

struct StringTableEntry {
  
  int address;
  string data;
  
};

typedef vector<StringTableEntry> StringTableEntryCollection;

void fixCase(string& str,
             bool capAfterPunct = true,
             bool capEveryWord = false) {
  string fixed;
  
  bool needCapital = true;
  for (int i = 0; i < str.size(); i++) {
    if (isalpha(str[i])) {
      if (needCapital) {
        fixed += toupper(str[i]);
        needCapital = false;
      }
      else {
        fixed += tolower(str[i]);
      }
    }
    else if (capAfterPunct
             && ((str[i] == '.')
                 || (str[i] == '!')
                 || (str[i] == '?'))) {
      needCapital = true;
      fixed += str[i];
    }
    else if (capEveryWord && isspace(str[i])) {
      needCapital = true;
      fixed += str[i];
    }
    else {
      fixed += str[i];
    }
  }
  
  str = fixed;
}

int main(int argc, char* argv[]) {
  
  if (argc < 4) {
    cout <<
    "tablerip: Generic string table extractor\n"
    "Usage: tablerip <file> <start> <end>\n"
    "<file> is the name of the input file.\n"
    "<start> is the offset of the table start.\n"
    "<end> is the offset of the table end (one past the last terminator).\n";
    return 0;
  }
  
  string filename = string(argv[1]);
  int tablestart = TStringConversion::stringToInt(string(argv[2]));
  int tableend = TStringConversion::stringToInt(string(argv[3]));
  
  ifstream ifs(filename.c_str(), ios_base::binary);
  int fileSize = fsize(ifs);
  TByte* buffer = new TByte[fileSize];
  ifs.read((char*)buffer, fileSize);
  
  StringTableEntryCollection strings;
  int pos = tablestart;
  while (pos < tableend) {
    StringTableEntry entry;
    entry.address = pos;
    entry.data = string((char*)(buffer + pos));
    strings.push_back(entry);
    pos += entry.data.size() + 1;
  }
  
  cout << "File: " << filename << endl << endl;
  cout << "Messages: " << strings.size() << endl << endl;
  
  if (changeCase) {
    for (int i = 0; i < strings.size(); i++) {
      fixCase(strings[i].data, punctCaps, spaceCaps);
    }
  }
  
  for (int i = 0; i < strings.size(); i++) {
    cout << "=== " << hex << i
         << " | "
         << strings[i].address
         << " | " << strings[i].data.size()
         << " ===" << endl;
    cout << strings[i].data;
    cout << "[END]";
    cout << endl << endl;
  } 
  
  delete buffer;
  
  return 0;
}  
