#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
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

int main(int argc, char* argv[]) {
  
  if (argc < 2) {
    return 0;
  }
  
  ifstream ifs(argv[1], ios_base::binary);
  int sz = fsize(ifs);
  char* buffer = new char[sz];
  ifs.read(buffer, sz);
  
  for (int i = 0; i < sz; i++) {
    int val = (unsigned char)(buffer[i]);
    if (val & 0x80) {
      val = 0x100 - (val & 0x7F);
    }
    buffer[i] = val;
  }
  
  ofstream ofs((argc >= 3)
                 ? (argv[2])
                 : (string(argv[1]) + "_sam").c_str(),
                ios_base::binary);
  ofs.write(buffer, sz);
  
  delete buffer;
  
}
