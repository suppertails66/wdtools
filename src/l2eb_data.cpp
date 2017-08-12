#include <iostream>
#include <fstream>
#include <string>
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"

#if defined(__linux__) || defined(__APPLE__)
  #include <sys/stat.h>
#elif _WIN32
  #include <windows.h>
#endif

using namespace std;
using namespace BlackT;

const static int sectorSize = 0x800;

string getDirectory(string filepath) {
  for (int i = filepath.size() - 1; i >= 0; i--) {
    if (filepath[i] == '/') {
      return filepath.substr(0, i);
    }
  }
  
  return string();
}

string getFilename(string filepath) {
  for (int i = filepath.size() - 1; i >= 0; i--) {
    if (filepath[i] == '/') {
      return filepath.substr(i + 1, filepath.size() - (i + 1));
    }
  }
  
  return string();
}

void createDirectory(string name) {
  if (name.size() != 0) {
    // first, create every parent directory, since mkdir
    // will not create subdirectories
    createDirectory(getDirectory(name));
    
    #if defined(__linux__) || defined(__APPLE__)
      mkdir(name.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
    #elif _WIN32
	    CreateDirectoryA(name.c_str(), NULL);
    #endif
  }
}

void createDirectoryForFile(string name) {
  createDirectory(getDirectory(name));
}

char* get_opt(char** begin, char** end, const std::string option) {
  char** idx = std::find(begin, end, option);
  if (idx != end && ++idx != end) {
    return *idx;
  } else {
    return 0;
  }
}

int main(int argc, char* argv[]) {
  bool upd_available = false;
  ifstream upd;
  char* upd_name;
  char* prefix_c;

  if (argc < 2) {
    printf("No IDX specified!\n");
    return 1;
  }
  if (argc < 3) {
    printf("No PAK specified!\n");
    return 1;
  }
  upd_name = get_opt(argv, argv + argc, "--upd");
  if (upd_name == 0) {
    printf("No UPD specified! Files will be extracted with generic names.\n");
  } else {
    upd_available = true;
    upd.open(upd_name, ios_base::binary);
  }
  
  ifstream idx(argv[1], ios_base::binary);
  ifstream pak(argv[2], ios_base::binary);
  
  string prefix;
  prefix_c = get_opt(argv, argv + argc, "--prefix");
  if (prefix_c != 0) {
    prefix = string(prefix_c);
  }
  
  // get filename table pointer and seek to it
  if (upd_available) {
    upd.seekg(0x10);
    upd.seekg(TSerialize::readInt(
      upd, 4, EndiannessTypes::little, SignednessTypes::nosign));
  }
  
  int fileno = 0;

  // read entries until all data has been read
  while (true) {
    char next = idx.get();
    if (!idx.good()) break;
    idx.unget();
    
    int sectorNum = TSerialize::readInt(
      idx, 3, EndiannessTypes::big, SignednessTypes::nosign);
    int numSectors = TSerialize::readInt(
      idx, 2, EndiannessTypes::big, SignednessTypes::nosign);
    
    string filename = prefix;
    if (upd_available) {
      do {
        next = upd.get();
        filename += next;
      } while (next != 0);
    } else {
      filename += std::to_string(fileno);
    }
    
    // replace backslashes
    for (int i = 0; i < filename.size(); i++) {
      if (filename[i] == '\\') filename[i] = '/';
    }
    std::cout << "Unpacking " << filename << "\n";
    
    createDirectoryForFile(filename);
    
    pak.seekg(sectorNum * sectorSize);
    int realSize = numSectors * sectorSize;
    char* buffer = new char[realSize];
    pak.read(buffer, realSize);
    
    ofstream ofs(filename.c_str());
    ofs.write(buffer, realSize);
    
    delete buffer;

    fileno += 1;
  }
  
  return 0;
}
