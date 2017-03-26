#include <iostream>
#include <fstream>

using namespace std;

int fsize(istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

int main(int argc, char* argv[]) {
  
  if (argc < 4) return 0;
  
  ifstream us(argv[1], ios_base::binary);
  ifstream jp(argv[2], ios_base::binary);
  ofstream ofs(argv[3], ios_base::binary);
  
  int num = 0;
  while (us.good()) {
    char bufferUS[0x18];
    char bufferJP[0x18];
    
    us.read(bufferUS, 0x18);
    jp.read(bufferJP, 0x18);
    
    bool found = false;
    for (int i = 4; i < 0x18; i++) {
      if (i == 0) continue;
      
      if (bufferUS[i] != bufferJP[i]) {
        found = true;
        cout << (num * 0x18) << " " << i << ": "
          << (int)((unsigned char*)(bufferJP))[i] << " "
          << (int)((unsigned char*)(bufferUS))[i] << endl;
      }
    }
    
    if (found) cout << endl;
    
    ofs.write(bufferUS, 4);
    ofs.write(bufferJP + 4, 0x18 - 4);
    
    ++num;
  }
  
  return 0;
}
