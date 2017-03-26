#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "util/TByte.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"
#include "scriptrip_tss_utils.h"

using namespace std;
using namespace BlackT;

int main(int argc, char* argv[]) {
  
  if (argc < 2) {
    cout <<
    "scriptrip: Lunar: The Silver Star (Sega CD) English dialogue extractor\n"
    "Usage: scriptrip <map> [subs]\n"
    "<map> is a Lunar: The Silver Star map file (e.g. C001.MAP).\n"
    "[subs] is an optional text file specifying strings to replace in"
    " the output.\n";
    return 0;
  }
  
  string filename = string(argv[1]);
  
  ifstream ifs(filename.c_str(), ios_base::binary);
  int fileSize = fsize(ifs);
  TByte* buffer = new TByte[fileSize];
  ifs.read((char*)buffer, fileSize);
           
  SubstitutionFile subs;                     
  if (argc >= 3) {
    ifstream subifs(argv[2]);
    subs.load(subifs);
  }
  
//  ScriptChunks scriptChunks;
//  int pos = mapObjectBlockAddress;
//  scriptChunks.load(buffer, pos, fileSize);

  int blockStart = ByteConversion::fromBytes(buffer + block0PointerOffset,
                                             4,
                                             EndiannessTypes::big,
                                             SignednessTypes::nosign);
  int blockEnd = ByteConversion::fromBytes(buffer + block1PointerOffset,
                                             4,
                                             EndiannessTypes::big,
                                             SignednessTypes::nosign);
  
  ScriptMessageFile scriptMessageFile;
  scriptMessageFile.generate(buffer, blockEnd, blockStart);
  
  for (int i = 0; i < scriptMessageFile.scriptMessages.size(); i++) {
    subs.doSubstitutions(
      scriptMessageFile.scriptMessages[i].data);
  }
  
  scriptMessageFile.print(filename);
  
/*  int blockEnd2 = ByteConversion::fromBytes(buffer + block1PointerOffset + 4,
                                             4,
                                             EndiannessTypes::big,
                                             SignednessTypes::nosign);
  for (int i = blockEnd; i < blockEnd2; i++) {
    cout << ((char*)buffer)[i];
  } */
  
  delete buffer;
  
  return 0;
} 
