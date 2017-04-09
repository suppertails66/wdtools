#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"

using namespace std;
using namespace BlackT;

const static int sectorSize = 0x800;

struct LSSSIndexEntry {
  
  char filename[16];
  int a;
  int sectornum;
  int filesize;
  int b;
  
  void read(istream& ifs) {
    ifs.read(filename, sizeof(filename));
    a = TSerialize::readInt(ifs, 4,
                EndiannessTypes::little, SignednessTypes::nosign);
    sectornum = TSerialize::readInt(ifs, 4,
                EndiannessTypes::little, SignednessTypes::nosign);
    filesize = TSerialize::readInt(ifs, 4,
                EndiannessTypes::little, SignednessTypes::nosign);
    b = TSerialize::readInt(ifs, 4,
                EndiannessTypes::little, SignednessTypes::nosign);
  }
};

struct LSSSIndex {
  
  int a;
  char unknown[16];
  int numentries;
  int filesize;
  int b;
  vector<LSSSIndexEntry> entries;
  
  void read(istream& ifs) {
    
    a = TSerialize::readInt(ifs, 4,
                EndiannessTypes::little, SignednessTypes::nosign);
    ifs.read(unknown, sizeof(unknown));
    numentries = TSerialize::readInt(ifs, 4,
                EndiannessTypes::little, SignednessTypes::nosign);
    filesize = TSerialize::readInt(ifs, 4,
                EndiannessTypes::little, SignednessTypes::nosign);
    b = TSerialize::readInt(ifs, 4,
                EndiannessTypes::little, SignednessTypes::nosign);
    
    for (int i = 0; i < numentries; i++) {
      LSSSIndexEntry entry;
      entry.read(ifs);
      entries.push_back(entry);
    }
  }
  
  void extract(istream& ifs, string prefix) {
    for (unsigned int i = 0; i < entries.size(); i++) {
      LSSSIndexEntry& entry = entries[i];
      ifs.seekg(entry.sectornum * sectorSize);
      char* buffer = new char[entry.filesize];
      ifs.read(buffer, entry.filesize);
      ofstream ofs(prefix + string(entry.filename), ios_base::binary);
      ofs.write(buffer, entry.filesize);
      delete buffer;
    }
  }
  
};

int main(int argc, char* argv[]) {
  if (argc < 2) return 0;
  
  ifstream ifs(argv[1], ios_base::binary);
  
  LSSSIndex index;
  index.read(ifs);
  
  string prefix;
  if (argc >= 3) prefix = string(argv[2]);
  index.extract(ifs, prefix);
  
  return 0;
}
