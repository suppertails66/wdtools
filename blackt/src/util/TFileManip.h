#ifndef TFILEMANIP_H
#define TFILEMANIP_H


#include <string>
#include <iostream>
#include <fstream>
#include "util/TByte.h"
#include "util/TArray.h"

namespace BlackT {


class TFileManip {
public:
  static unsigned int fileExists(const std::string& filename);
  
  static unsigned int getFileSize(std::istream& ifs);
  
  static void readEntireFile(TArray<TByte>& dst,
                             const std::string& filename);
  
  static void readEntireStream(TArray<TByte>& dst,
                              std::istream& ifs);
protected:
  
};


};


#endif
