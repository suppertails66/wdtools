#include <iostream>
#include <fstream>
#include <string>
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

int readStats(istream& ifs, char** p) {
  ifs.seekg(4, ios_base::cur);
  int statLen = TSerialize::readInt(
    ifs, 4, EndiannessTypes::little, SignednessTypes::nosign);
  *p = new char[statLen];
  ifs.read(*p, statLen);
  return statLen;
}

int main(int argc, char* argv[]) {
  
  if (argc < 3) return 0;
  
  ifstream ifsUS(argv[1], ios_base::binary);
  ifstream ifsJP(argv[2], ios_base::binary);
  
  char* bufferUS;
  char* bufferJP;
  
  int sizeUS = readStats(ifsUS, &bufferUS);
  int sizeJP = readStats(ifsJP, &bufferJP);
  
  cout << sizeJP << endl;
  for (int i = 0; i < sizeJP; i++) {
    if (bufferJP[i] != bufferUS[i]) {
      cout << hex << i << ": " << (int)((unsigned char)(bufferJP[i])) << " -> "
        << (int)((unsigned char)(bufferUS[i])) << endl;
    }
  }
  
  delete bufferUS;
  delete bufferJP;
  
}
