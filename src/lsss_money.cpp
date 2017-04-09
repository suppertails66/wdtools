#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "util/TSerialize.h"
#include "util/ByteConversion.h"

using namespace std;
using namespace BlackT;

struct MoneyOffset {
  
  int addr;
  int pad;
  int arg2;
  int amount;
  
  bool match(const char* src, int pos, int srcSize) {
    int remaining = srcSize - pos;
    if (remaining < 8) return false;
    
    if (src[pos] != 0x1C) return false;
    if (src[pos + 1] != 0x00) return false;
    
    // 32-bit alignment required so CPU can read amount as a word
    int nextpos = ((pos + 2) & -4) + 4;
    
    if ((nextpos + 4) >= srcSize) return false;
    
/*    int realPadDist = nextpos - pos - 4;
    if (realPadDist > 0) {
      for (int i = nextpos - realPadDist; i < nextpos; i++) {
        if (src[i] != 0x00) return false;
      }
    } */
    
//    for (int i = 1; i < 4; i++) {
//      if (src[pos + i] != 0x00) return false;
//    }
    
    addr = pos;
    pad = (nextpos - pos);
    arg2 = ByteConversion::fromBytes(src + pos + 2, 2,
                                     EndiannessTypes::little,
                                     SignednessTypes::nosign);
    amount = ByteConversion::fromBytes(src + nextpos, 4,
                                       EndiannessTypes::little,
                                       SignednessTypes::nosign);
                                       
    if ((unsigned int)amount > 200000) return false;
    if (amount % 5) return false;
                                       
    return true;
    
    
    
  }
  
  void print(ostream& ofs) {
    ofs << "Addr: " << hex << addr << endl;
    ofs << "Pad: " << dec << pad << endl;
    ofs << "arg2: " << arg2 << endl;
    ofs << "Amount: " << amount << endl;
    ofs << endl;
  }
  
};

typedef vector<MoneyOffset> MoneyOffsetCollection;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

void readMoneyOffsets(MoneyOffsetCollection& dst, const char* src,
                      int srcSize) {
  for (int i = 0; i < srcSize; i += 2) {
    MoneyOffset offset;
    if (offset.match(src, i, srcSize)) {
      dst.push_back(offset);
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 4) return 0;
  
  ifstream ifsUS(argv[1], ios_base::binary);
  ifstream ifsJP(argv[2], ios_base::binary);
  
  int szUS = fsize(ifsUS);
  int szJP = fsize(ifsJP);
  
  char* bufferUS = new char[szUS];
  char* bufferJP = new char[szJP];
  
  ifsUS.read(bufferUS, szUS);
  ifsJP.read(bufferJP, szJP);
  
  MoneyOffsetCollection offsetsUS;
  MoneyOffsetCollection offsetsJP;
  
  readMoneyOffsets(offsetsUS, bufferUS, szUS);
  readMoneyOffsets(offsetsJP, bufferJP, szJP);
  
  cout << offsetsJP.size() << " " << offsetsUS.size() << endl;
  cout << "===JP===" << endl;
  for (int i = 0; i < offsetsJP.size(); i++) {
    offsetsJP[i].print(cout);
  }
  cout << "===US===" << endl;
  for (int i = 0; i < offsetsUS.size(); i++) {
    offsetsUS[i].print(cout);
  }
  cout << endl << endl;
  
  delete bufferUS;
  delete bufferJP;
  
//  ofstream ofs(argv[3], ios_base::binary);
  
  return 0;
}
