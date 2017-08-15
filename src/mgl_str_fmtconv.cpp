#include "mgl_transtxt.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {

  if (argc < 3) {
    return 0;
  }
  
  string transFileName(argv[1]);
  string outFileName(argv[2]);
  
  // read translation file
  ifstream ifs(transFileName.c_str());
  TranslationFile translationFile;
  translationFile.load(ifs);
//  translationFile.loadNew(ifs);
  
  ofstream ofs(outFileName.c_str());
  
  // write column headers
  ofs << "file,offset,pointers,size,japanese,english,comments" << endl;
  
  // write new format
  translationFile.saveNew(ofs);
  
  return 0;
}
