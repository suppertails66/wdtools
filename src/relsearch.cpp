#include <iostream>
#include <fstream>
#include <string>

using namespace std;
//using namespace BlackT;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

void relSearch(const string& content, string searchstr, int offset) {
  for (int i = 0; i < searchstr.size(); i++) {
    searchstr[i] += offset;
  }
  
  for (int i = 0; i < content.size() - searchstr.size() + 1; i++) {
    if (content.substr(i, searchstr.size()).compare(searchstr) == 0) {
      cout << hex << i << " " << offset << endl;
    }
  }
}

int main(int argc, char* argv[]) {
  
  if (argc < 3) {
    cout << "Usage: relsearch <file> <string>" << endl;
    return 0;
  }
  
  string filename(argv[1]);
  string searchstr(argv[2]);
  
  ifstream ifs(filename.c_str(), ios_base::binary);
  int filesize = fsize(ifs);
  if (filesize <= 0) {
    return 0;
  }
  
  char* buffer = new char[filesize];
  ifs.read(buffer, filesize);
  string content(buffer, filesize);
  delete buffer;
  
  for (int i = 0; i < 256; i++) {
    relSearch(content, searchstr, i);
  }
  
/*  for (int i = 0; i < content.size(); i++) {
    content[i] -= 0xbb;
  }
  ofstream ofs((filename + "_offset").c_str(), ios_base::binary);
  ofs.write(content.c_str(), content.size()); */
  
  return 0;
}
