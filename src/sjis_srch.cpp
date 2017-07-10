#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

bool between(int value, int lower, int upper) {
  return (((value < lower) || (value > upper)) ? false : true);
}

int checkSjisString(const unsigned char* buffer, int pos, int sz) {
  int len = 0;
  
  // assume no intermixed full/halfwidth sequences
  bool halfwidth = false;
  if (between(buffer[pos], 0xA0, 0xDF)) {
    halfwidth = true;
  }
  
  while (pos + len < sz) {
    unsigned char next = buffer[pos + len];
    
//    if (pos == 0x170B4) cerr << hex << (unsigned int)next << endl;
    
    if (next == 0) break;
    
//    if (next < 0x20) return -1;
    
    if (halfwidth) {
      if (!between(next, 0xA0, 0xDF)) return -1;
      
      ++len;
    }
    else {
      // need 2 chars
      if (pos + len + 1 >= sz) return -1;
      
      if (!(between(next, 0x20, 0x40)
          || between(next, 0x81, 0x9F)
          || between(next, 0xE0, 0xF0))) return -1;
      
      if (next == 0x20) ++len;
      else len += 2;
    }
    
  }
  
  if (len == 0) return -1;
  
  if (pos + len > sz) len = (sz - pos);
  
  return len;
}

int main(int argc, char* argv[]) {
  if (argc < 2) return 0;
  
  ifstream ifs(argv[1], ios_base::binary);
  int sz = fsize(ifs);
  
  char* buffer = new char[sz];
  ifs.read(buffer, sz);
  
  int pos = 0;
  while (pos < sz) {
    int len = checkSjisString((unsigned char*)buffer, pos, sz);
    if (len == -1) ++pos;
    else {
      string str(buffer + pos, len);
      cout << hex << setw(6) << pos << "|" << str << endl;
      pos += len + 1;
    }
  }
  
  delete buffer;
  
  return 0;
}
