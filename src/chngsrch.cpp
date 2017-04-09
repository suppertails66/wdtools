#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cmath>
#include "util/ByteConversion.h"
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

char* readFile(ifstream& ifs) {
  int sz = fsize(ifs);
  char* buffer = new char[sz];
  ifs.read(buffer, sz);
  return buffer;
}

int main(int argc, char* argv[]) {
  
  if (argc < 5) {
    
    return 0;
  }
  
  ifstream ifs1(argv[1], ios_base::binary);
  ifstream ifs2(argv[2], ios_base::binary);
  int sz1 = fsize(ifs1);
  int sz2 = fsize(ifs2);
  char* file1 = readFile(ifs1);
  char* file2 = readFile(ifs2);
  int orig = TStringConversion::stringToInt(string(argv[3]));
  int chng = TStringConversion::stringToInt(string(argv[4]));
  
  int sz = min(sz1, sz2);
  for (int i = 0; i < sz; i++) {
    int b1 = ((unsigned char*)file1)[i];
    int b2 = ((unsigned char*)file2)[i];
    
    if ((b1 == orig) && (b2 == chng)) {
      cout << hex << i << endl;
    }

//    if (b1 + orig == b2) {
//      cout << hex << i << endl;
//    }
  }
  
  delete file1;
  delete file2;
  
  return 0;
}
