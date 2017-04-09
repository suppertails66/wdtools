#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "util/TStringConversion.h"

using namespace std;
using namespace BlackT;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

typedef vector<string> LsssStringTable;

void generateLsssStringTable(LsssStringTable& dst, const unsigned char* src, int sz) {
  int pos = 0;
  while (pos < sz) {
    string next((char*)src + pos);
    dst.push_back(next);
    pos += next.size() + 1;
  }
}

void realign(int& pos, int alignment) {
  pos = (pos & -alignment) + alignment;
}

bool makeLsssHeader(const unsigned char* src, int& pos, string& dst) {
  int startingPos = pos;
  
//  realign(pos, 2);
  char next = src[pos++];
  while (next != 0x0E) {
    
    switch ((unsigned char)next) {
    case 0xF8:
      {
      int code = (unsigned char)(src[pos++]);
      dst += "[F8:";
      dst += TStringConversion::toString(code);
      dst += "]";
      }
      break;
    case 0xFA:
      {
      int code = (unsigned char)(src[pos++]);
      dst += "[POR";
      dst += TStringConversion::toString(code);
      dst += "]";
      }
      break;
    case 0xFB:
      {
      int code = (unsigned char)(src[pos++]);
      dst += "[ROR";
      dst += TStringConversion::toString(code);
      dst += "]";
      }
      break;
    case 0xFF:
      // allow makeLsssString to detect
      --pos;
      return true;
      break;
    default:
      cerr << hex << (pos - 1) << ": unknown lsss header "
           << hex << (int)((unsigned char)next) << endl;
      pos = startingPos;
      return false;
      break;
    }
    
    next = src[pos++];
  }
  
//  if (pos % 2) realign(pos, 2);
  return true;
}

int convertLsssCharacter(const unsigned char* src, int pos, int srcsize,
                         string& dst, const LsssStringTable& strings) {
  unsigned char next = src[pos++];
  
  if (next == 0xFF) {
    dst += "[FF]";
    return pos;
  }
  else if (next == 0) {
    dst += "[00]";
    return pos;
  }
  // character literal
  else if (next < 0x5C) {
    // convert to ASCII
    char nextreal = next + 0x1F;
    
    dst += nextreal;
    
    // end of dialogue
    if (nextreal == '$') {
      
    }
    // end of current box
    else if (nextreal == '*') {
      
    }
  }
  // comression string lookup
  else {
    if ((pos >= srcsize)
        && ((next == 0xFD)
            || (next == 0xFE))) {
      dst += next;
      return pos;
    }
    
    int index;
    switch (next) {
    // extended lookup 1
    case 0xFD:
      index = 0x0A1 + src[pos++];
      break;
    // extended lookup 2
    case 0xFE:
      index = 0x19E + src[pos++];
      break;
    // standard lookup
    default:
      index = next - 0x5C;
      break;
    }
    
    if (index >= strings.size()) {
//      cerr << "out of range lookup: " << index << endl;
      dst += next;
      return pos;
    }
    
    dst += strings[index];
  }
  
  // return updated pos
  return pos;
}

int makeLsssString(const unsigned char* src, int pos, string& dst,
                    const LsssStringTable& strings) {
  if (!makeLsssHeader(src, pos, dst)) return -1;
  
  if (src[pos - 1] == 0xFF) {
    cerr << "encountered ff" << endl;
    return pos;
  }
  
  while (true) {
    unsigned char next = src[pos++];
    
    if (next == 0xFF) {
//      cerr << "encountered ff (1)" << endl;
      dst += "[FF]";
      break;
    }
    
    if (next == 0) {
      if (!makeLsssHeader(src, pos, dst)) return -1;

      if (src[pos - 1] == 0xFF) {
        cerr << "encountered ff (2)" << endl;
        return pos;
      }
    }
    // character literal
    else if (next < 0x5C) {
      // convert to ASCII
      char nextreal = next + 0x1F;
      
      dst += nextreal;
      
      // end of dialogue
      if (nextreal == '$') {
        break;
      }
      // end of current box
      else if (nextreal == '*') {
/*        if (!makeLsssHeader(src, pos, dst)) return -1;

        if (src[pos - 1] == 0xFF) {
          cerr << "encountered ff (2)" << endl;
          return pos;
        } */
      }
    }
    // comression string lookup
    else {
      
      
      int index;
      switch (next) {
      // extended lookup 1
      case 0xFD:
        index = 0x0A1 + src[pos++];
        break;
      // extended lookup 2
      case 0xFE:
        index = 0x19E + src[pos++];
        break;
      // standard lookup
      default:
        index = next - 0x5C;
        break;
      }
      
      if (index >= strings.size()) {
        cerr << "out of range lookup: " << index << endl;
        return -1;
      }
      
      dst += strings[index];
    }
  }
  
  // return updated pos
  return pos;
}

int main(int argc, char* argv[]) {
  if (argc < 3) return 0;
  
  ifstream ifs(argv[1], ios_base::binary);
  int sz = fsize(ifs);
  char* buffer = new char[sz];
  ifs.read(buffer, sz);
  
  LsssStringTable strings;
  generateLsssStringTable(strings, (unsigned char*)buffer, sz);
  
//  for (int i = 0; i < strings.size(); i++) {
//    cout << i << ": " << strings[i] << endl;
//  }
  
  ifs.close();
  ifs.open(argv[2]);
  int szN = fsize(ifs);
  char* bufferN = new char[szN];
  ifs.read(bufferN, szN);
  
//  string test;
//  bool result = makeLsssString(bufferN, 0x1416, test, strings);
//  cout << test << endl;

//  string test;
//  int result = makeLsssString((unsigned char*)bufferN, 0x23a4 + 2, test, strings);
//  cout << test << endl;

  int i = 0;

  // search for all messages starting with msg opcode (02 00)
/*  vector<string> stringsN;
  vector<int> addressesN;
  vector<int> addressesEndN;
  
  i = 0;
  while (i < szN - 1) {
    if ((bufferN[i] == 0x02) && (bufferN[i + 1] == 0x00)) {
      i += 2;
      string str;
      int newpos = makeLsssString((unsigned char*)bufferN, i, str, strings);
      if (newpos != -1) {
        stringsN.push_back(str);
        addressesN.push_back(i);
        addressesEndN.push_back(newpos);
        
        // maintain 16-bit alignment
        if (newpos % 2) realign(newpos, 2);
        i = newpos;
      }
    }
    else {
      i += 2;
    }
  }
  
  for (int i = 0; i < stringsN.size(); i++) {
    cout << hex << addressesN[i] << "-" << addressesEndN[i]
         << ": " << stringsN[i] << endl;
  } */
  
  // search for all messages formatted as standard strings (starting with 0x0E)
  vector<string> rawStrings;
  vector<int> rawStringAddresses;
  vector<int> rawStringAddressesEnd;
  
  i = 0;
  while (i < szN - 1) {
    if ((bufferN[i] == 0x0E)) {
      string str;
      int newpos = makeLsssString((unsigned char*)bufferN, i, str, strings);
      if (newpos != -1) {
        rawStrings.push_back(str);
        rawStringAddresses.push_back(i);
        rawStringAddressesEnd.push_back(newpos);
        i = newpos;
      }
      else {
        i++;
      }
    }
    else {
      i++;
    }
  }
  
  for (int i = 0; i < rawStrings.size(); i++) {
    cout << hex << rawStringAddresses[i] << "-" << rawStringAddressesEnd[i]
         << ": " << rawStrings[i] << endl;
  } 
  
  // convert all characters as-is, ignoring formatting
/*  string all;
  i = 0;
  while (i < szN - 1) {
    i = convertLsssCharacter((unsigned char*)bufferN, i, szN, all, strings);
  }
  cout << all; */
  
  delete buffer;
  delete bufferN;
  
  return 0;
}
