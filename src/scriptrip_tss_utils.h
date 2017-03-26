#ifndef SCRIPTRIP_TSS_UTILS_H
#define SCRIPTRIP_TSS_UTILS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "util/TByte.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"
  
// If true, the program tries to heuristically guess proper case.
// Otherwise, it outputs the raw text.
const static bool changeCase = true;

const static int block0PointerOffset = 0x00000000;
const static int block1PointerOffset = 0x00000004;
const static int block2PointerOffset = 0x00000008;

typedef std::string RawData;
typedef std::vector<RawData> RawDataCollection;

struct SubstitutionFileEntry {
  
  std::string orig;
  std::string sub;
  
  void trySubstitute(std::string& str);
  
  void trySubstitutes(std::string& str, std::string origReal, std::string subReal);
  
  void tryAltSubstitutes(std::string& str, std::string origReal,
                         std::string subReal,
                         char orig, char sub);
  
  std::string substituteChars(std::string& origStr,
                      char orig, char sub);
  
};

struct SubstitutionFile {
  
  typedef std::vector<SubstitutionFileEntry> SubstitutionFileEntryCollection;
  
  void load(std::istream& ifs);
  
  void doSubstitutions(std::string& str);
  
  SubstitutionFileEntryCollection entries;
};

struct RawChunk {
  int address;
  RawData chunkData;
};
  
struct ScriptChunks {
  typedef std::vector<RawChunk> RawChunkCollection;
  
  void load(BlackT::TByte* src, int& pos, int fileSize);
  
  int findNextChunkAddress(int index, int fileSize);
  
  RawChunkCollection chunks;
};

struct ScriptMessage {
  
  int address;
  int length;
  std::string data;
  
};

struct ScriptMessageFile {
  
  typedef std::vector<ScriptMessage> ScriptMessageCollection;
  
  enum MessageType {
    messageNone = 0,
    messageAttributed,
    messageUnattributed,
    messagePointedString,
    messageUnknown1,
    messageUnknown2
  };
  
//  void generate(ScriptChunks& scriptChunks) {
  void generate(BlackT::TByte* src, int srcSize, int startAddress,
                    int threshold = 1500,
                    int messageTotalWeight = 80,
                    int messageSizeWeight = 166);
  
  void addScriptMessages(BlackT::TByte* src, int srcEnd, int startAddress,
                    int threshold,
                    int messageTotalWeight,
                    int messageSizeWeight);
  
  void substituteMessages();
  
  void substituteMessage(ScriptMessage& message);
  
  int addBasicStringMessage(BlackT::TByte* realSrc, int srcEnd,
                            int messageRealAddress);
  
  int addBasicStringMessage(BlackT::TByte* realSrc, int srcEnd,
                            int messageRealAddress,
                            ScriptMessageCollection& dst);
  
  int getNextIndexAddress(RawChunk& chunk, int& pos);
  
  int getNextIndexOffset(RawChunk& chunk, int& pos);
  
  void print(std::string filename = "");
  
  ScriptMessageCollection scriptMessages;
};

int fsize(std::istream& ifs);

#endif
