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

void printCompared(int first, int second) {

  cout << " | align=\"center\" | ";
  if (first == second) {
    cout << "<div style=\"color: gray;\">" << dec << first << "</div>\n";
  }
  else {
    cout << "<div style=\"font-weight: bold\">";
    cout << dec << first << " -> " << second << "";
  
    if (second > first) {
      cout << "<div style=\"color: red;\">(";
      cout << dec << "+" << second - first << ")";
    }
    else {
      cout << "<div style=\"color: green;\">(";
      cout << dec << second - first << ")";
    }
    
    cout << "</div></div>\n";
  }
}

int main(int argc, char* argv[]) {
  
  if (argc < 3) return 0;
  
  ifstream us(argv[1], ios_base::binary);
  ifstream jp(argv[2], ios_base::binary);
//  ofstream ofs(argv[3], ios_base::binary);
  
        
  cout << "{| class=\"wikitable\" style=\"margin: 1em auto 1em auto;\"\n"
       << " ! Offsets\n"
       << " ! Attack\n"
       << " ! ?\n";
       
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
        
/*        cout << (num * 0x18) << " " << i << ": "
          << (int)((unsigned char*)(bufferJP))[i] << " "
          << (int)((unsigned char*)(bufferUS))[i] << endl; */
        
        
      }
    }
    
//    if (found) cout << endl;
  if (found) {
    cout << " |-\n";
    cout << " | align=\"center\" | "
         << "JP: {{hex|0x"
         << hex << uppercase << (num * 0x18) + 0x67858 << "}}\n"
         << "US: {{hex|0x"
         << hex << uppercase << (num * 0x18) + 0x689D4 << "}}\n";
    printCompared((unsigned char)(bufferUS[5]), (unsigned char)(bufferJP[5]));
    printCompared((unsigned char)(bufferUS[4]), (unsigned char)(bufferJP[4]));
  }
    
/*    ofs.write(bufferUS, 4);
    ofs.write(bufferJP + 4, 0x18 - 4); */
    
    ++num;
  }
  
  cout << " |}\n";
  
  return 0;
}
