#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "util/TByte.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"
#include "scriptrip_tss_utils.h"

const static int maxOutfileSize = 0x100000;

using namespace std;
using namespace BlackT;

void buildIndex(char* src, vector<int>& dst) {
  int first = ByteConversion::fromBytes(src + 0,
                                        4,
                                        EndiannessTypes::big,
                                        SignednessTypes::nosign);
  for (int i = 0; i < first; i += 4) {
    int value = ByteConversion::fromBytes(src + i,
                                        4,
                                        EndiannessTypes::big,
                                        SignednessTypes::nosign);
    dst.push_back(value);
  }
}

void buildSizeIndex(vector<int>& index, vector<int>& dst, int filesize) {
  for (int i = 1; i < index.size(); i++) {
    dst.push_back(index[i] - index[i - 1]);
  }
  
  dst.push_back(filesize - index[index.size() - 1]);
}

//const static int targetJPChunk = (0x2C / 4);
//const static int targetJPChunk = (0x8 / 4);
//const static int targetJPChunk = 4;

struct OutputFile {
  
  char buffer[maxOutfileSize];
  int size;
  int headerPos;
  
  void addChunk(char* src, int srcSize) {
    memcpy(buffer + size, src, srcSize);
    ByteConversion::toBytes(size,
                            buffer + headerPos,
                            4,
                            EndiannessTypes::big,
                            SignednessTypes::nosign);
    headerPos += 4;
    size += srcSize;
  }
  
  void build(char* fileUS, char* fileJP,
             int sizeUS, int sizeJP,
             int targetJPChunk) {
    size = 0;
    headerPos = 0;
  
    vector<int> indexUS;
    vector<int> indexJP;
    
    buildIndex(fileUS, indexUS);
    buildIndex(fileJP, indexJP);
    
    vector<int> sizeIndexUS;
    vector<int> sizeIndexJP;
    
    buildSizeIndex(indexUS, sizeIndexUS, sizeUS);
    buildSizeIndex(indexJP, sizeIndexJP, sizeJP);
    
    size = indexUS[0];
    
    for (int i = 0; i < targetJPChunk; i++) {
      addChunk(fileUS + indexUS[i], sizeIndexUS[i]);
    }
    
    addChunk(fileJP + indexJP[targetJPChunk], sizeIndexJP[targetJPChunk]);
    
    for (int i = targetJPChunk + 1; i < indexUS.size(); i++) {
//    for (int i = targetJPChunk + 1; i < indexJP.size(); i++) {
      addChunk(fileUS + indexUS[i], sizeIndexUS[i]);
//      addChunk(fileJP + indexJP[i], sizeIndexJP[i]);
    }
  }
  
  void write(string filename) {
    ofstream ofs(filename.c_str(), ios_base::binary);
    ofs.write(buffer, size);
  }
  
};

int main(int argc, char* argv[]) {
  
  if (argc < 5) {
    return 0;
  }
  
  string fileNameUS(argv[1]);
  string fileNameJP(argv[2]);
  int targetJPChunk = TStringConversion::stringToInt(string(argv[3]));
  string outfile(argv[4]);
//  string substitutionFileName(argv[4]);
  
  ifstream ifsUS(fileNameUS.c_str());
  ifstream ifsJP(fileNameJP.c_str());
//  ifstream ifsSubs(substitutionFileName.c_str());
  
  int sizeUS = fsize(ifsUS);
  int sizeJP = fsize(ifsJP);
  
  char* fileUS = new char[sizeUS];
  char* fileJP = new char[sizeJP];
  
  ifsUS.read(fileUS, sizeUS);
  ifsJP.read(fileJP, sizeJP);
  
  OutputFile output;
  output.build(fileUS, fileJP, sizeUS, sizeJP, targetJPChunk);
  output.write(outfile);
  
//  ofstream ofs(outfile.c_str(), ios_base::binary);
//  ofs.write(fileUS, sizeUS);
  
  return 0;
}
