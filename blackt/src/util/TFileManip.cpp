#include "util/TFileManip.h"

namespace BlackT {


unsigned int TFileManip::fileExists(const std::string& filename) {
  std::ifstream ifs(filename,
                    std::ios_base::binary);
                    
  if ((int)(getFileSize(ifs)) <= 0) {
    return false;
  }
  
  return true;
}

unsigned int TFileManip::getFileSize(std::istream& ifs) {
  std::ifstream::pos_type pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  std::ifstream::pos_type size = ifs.tellg();
  ifs.seekg(pos);
  
  return size;
}
  
void TFileManip::readEntireFile(TArray<TByte>& dst,
                           const std::string& filename) {
  std::ifstream ifs(filename, std::ios_base::binary);
  readEntireStream(dst, ifs);
}
  
void TFileManip::readEntireStream(TArray<TByte>& dst,
                            std::istream& ifs) {
  int sz = getFileSize(ifs);
//  ifs.seekg(0, std::ios_base::beg);
  dst.resize(sz);
  ifs.read((char*)(dst.data()), sz);
}


};
