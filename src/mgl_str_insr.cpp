#include "mgl_transtxt.h"
#include "util/ByteConversion.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;
using namespace BlackT;

const static int kanjiLoadAddress = 0x2F2000;
const static int kanjiFreeSpaceStart = 0x2C48;
const static int kanjiFreeSpaceEnd = 0xD0A8;
const static char kanjiFileName[] = "KANJI.FNT";

// mgltrans srcfldr/ dstfldr/ transfile.txt datafile.txt freespace.txt 

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Usage:" << argv[0] << " srcpath dstpath [transfiles]" << endl;
    return 0;
  }
  
  string srcPrefix = argv[1];
  string dstPrefix = argv[2];
    
  // Read KANJI.FNT
  ifstream ifs((srcPrefix + kanjiFileName).c_str());
  int kanjiFileSize = fsize(ifs);
  char* kanjiBuffer = new char[kanjiFileSize];
  ifs.read(kanjiBuffer, kanjiFileSize);
  ifs.close();
  
  int kanjiPutPos = kanjiFreeSpaceStart;
  
  // Read all input translation files in sequence
  for (int i = 3; i < argc; i++) {
    string transFileName = argv[i];
    
    // Read translation file
    ifstream ifs(transFileName.c_str());
    TranslationFile translationFile;
    translationFile.load(ifs);
    ifs.close();
    
    // Handle each file
    for (FilenameTranslationEntryMap::iterator it
            = translationFile.filenameEntriesMap.begin();
         it != translationFile.filenameEntriesMap.end();
         ++it) {
      ifstream ifs((srcPrefix + it->first).c_str());
      int fileSize = fsize(ifs);
      char* fileBuffer = new char[fileSize];
      ifs.read(fileBuffer, fileSize);
      ifs.close();
      
      TranslationEntryCollection& entries = it->second;
      for (unsigned int i = 0; i < entries.size(); i++) {
        TranslationEntry& entry = entries[i];
        
        // place at original location if possible
  //      cerr << entry.originalSize << endl;
        if (entry.translatedText.size() <= entry.originalSize) {
          // can't use strcpy -- there may be embedded nulls in the text
          // that have to be preserved
          memcpy(fileBuffer + entry.sourceFileOffset,
                 entry.translatedText.c_str(),
                 entry.translatedText.size());
          
          // add terminator
          memset(fileBuffer + entry.sourceFileOffset
                  + entry.translatedText.size(),
                 0,
                 1);
        }
        // otherwise, move to KANJI.FNT
        else {
          int startPos = kanjiPutPos;
          kanjiPutPos += entry.translatedText.size() + 1;
          
          if (kanjiPutPos > kanjiFreeSpaceEnd) {
            cerr << "Error: Not enough space in KANJI.FNT" << endl;
            return 1;
          }
          
          cout << "Moved to KANJI.FNT, " << hex << startPos
            << ": " << entry.sourceFile << ", " << entry.sourceFileOffset << ", "
              << entry.translatedText << endl;
          
          memcpy(kanjiBuffer + startPos,
                 entry.translatedText.c_str(),
                 entry.translatedText.size());
          
          // add terminator
          memset(kanjiBuffer + startPos
                  + entry.translatedText.size(),
                 0,
                 1);
          
          // update pointers
          unsigned int newPointer = kanjiLoadAddress + startPos;
          
          for (unsigned int i = 0; i < entry.pointers.size(); i++) {
            char* dst = fileBuffer + entry.pointers[i];
            ByteConversion::toBytes(newPointer, dst, 4,
              EndiannessTypes::big, SignednessTypes::nosign);
          }
        }
      }
      
      ofstream ofs((dstPrefix + it->first).c_str(), ios_base::trunc);
      ofs.write(fileBuffer, fileSize);
      
      delete fileBuffer;
    }
  }
  
  // write KANJI.FNT
  ofstream ofs((dstPrefix + kanjiFileName).c_str());
  ofs.write(kanjiBuffer, kanjiFileSize);
  
  delete kanjiBuffer;
  
  return 0;
}
