#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include "util/TByte.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"

using namespace std;
using namespace BlackT;

typedef vector<unsigned int> IndexList;

const static int addrSize = 4;
const static bool offsetsInFilename = false;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

void generateIndex(IndexList& dst, char* src,
                   unsigned int chunksize) {
  while (true) {
    unsigned int offset = ByteConversion::fromBytes(src, addrSize,
                                       EndiannessTypes::big,
                                       SignednessTypes::nosign);
    src += addrSize;
    if ((offset == 0)
        || (offset >= chunksize)
        || ((dst.size() >= 1)
            && (offset < dst[dst.size() - 1]))) {
      if (offset == 0xFFFFFFFF) continue;
      break;
    }
    
    dst.push_back(offset);
  }
  
//  cout << dst.size() << endl;
}

void generateChunkIndex(IndexList& dst, char* src,
                        unsigned int chunksize) {
//  cerr << hex << chunksize << endl;
  unsigned int pos = 0;
  while (true) {
    if ((pos + addrSize > chunksize)
        || ((dst.size() > 0)
            && (pos >= dst[0]))) {
      break;
    }
    
    unsigned int offset = ByteConversion::fromBytes(src + pos, addrSize,
                                       EndiannessTypes::big,
                                       SignednessTypes::nosign);
    pos += addrSize;
//    cerr << hex << offset << endl;
    if ((offset == 0)
        || (offset >= chunksize)) {
      if (offset != 0xFFFFFFFF) break;
      else {
        dst.push_back(offset);
        continue;
      }
    }
    
    if (((dst.size() > 0)
            && (dst[dst.size() - 1] != 0xFFFFFFFF)
            && (offset < dst[dst.size() - 1]))) {
//      cerr << hex << offset << " " << hex << dst[(pos / 4) - 1] << endl;
//      char c; cin >> c;
      dst.clear();
      break;
    }
    
    dst.push_back(offset);
  }
  
  // there shouldn't be an index if there's only one subchunk
  if (dst.size() == 1) {
    dst.clear();
  }
}

unsigned int nextIndex(IndexList& index, int pos, int chunksize) {
  for (int i = pos + 1; i < index.size(); i++) {
    if (index[i] != 0xFFFFFFFF) {
      return (index[i] - index[pos]);
    }
  }
  
/*  (i == index.size() - 1)
                    ? (chunksize - index[i])
                    : (index[i + 1] - index[i]) */
  
  return chunksize - index[pos];
}

void writeChunks(char* src,
                 unsigned int chunksize,
                 unsigned int offset,
                 int chunknum,
                 string baseName) {
  
  IndexList index;
  generateChunkIndex(index, src, chunksize);
  
  // No subchunks
  if (index.size() == 0) {
    ofstream ofs;
    if (offsetsInFilename) {
      ofs.open(baseName
                    + "-"
                    + TStringConversion::intToString(offset,
                        TStringConversion::baseHex),
                    ios_base::binary);
    }
    else {
      ofs.open(baseName,
                   ios_base::binary);
    }
    ofs.write(src, chunksize);
  }
  else {
    for (int i = 0; i < index.size(); i++) {
//    for (int i = 0; i < 20; i++) {
      if (index[i] == 0xFFFFFFFF) {
        ofstream ofs((baseName + "-" + TStringConversion::toString(i)).c_str(),
                     ios_base::binary);
        continue;
      }

      writeChunks(src + index[i],
                  nextIndex(index, i, chunksize),
                  offset + index[i],
                  i,
                  baseName + "-" + TStringConversion::toString(i));
                  
    }
  }
  
}

int main(int argc, char* argv[]) {
  
  if (argc < 2) {
    return 0;
  }
  
  ifstream ifs(argv[1], ios_base::binary);
  int datfileSize = fsize(ifs);
  char* datfile = new char[datfileSize];
  ifs.read(datfile, datfileSize);
//  char* datfile = new char[0x8000];
//  ifs.read(datfile, 0x8000);

  int startPos;
  if (argc >= 3) {
    startPos = TStringConversion::stringToInt(argv[2]);
  }
  else {
    startPos = 0;
  }
  
  IndexList index;
  generateIndex(index, datfile + startPos, datfileSize - startPos);
  
//  for (int i = 0; i < index.size(); i++) {
//    cout << hex << index[i] << endl;
//  }

  string outbase;
  if (argc >= 4) {
    outbase = string(argv[3]);
  }
  else {
    outbase = string(argv[1]);
  }
  
/*  for (int i = 0; i < index.size(); i++) {
    string outname = outbase + "-"
                      + TStringConversion::toString(i)
                      + "-"
                      + TStringConversion::intToString(
                          index[i], TStringConversion::baseHex);
    
    int sz;
    if (i == index.size() - 1) sz = datfileSize - index[i];
    else sz = index[i + 1] - index[i];
    
    ofstream ofs(outname.c_str(), ios_base::binary);
    ofs.write(datfile + index[i], sz);
  } */
  
  writeChunks(datfile + startPos, datfileSize - startPos,
              startPos, 0, outbase);
  
  delete datfile;
  
  return 0;
} 
