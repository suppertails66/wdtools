#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "util/TByte.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"

using namespace std;
using namespace BlackT;

const static int mapObjectBlockPointerOffset = 0x08;

typedef string RawData;
typedef vector<RawData> RawDataCollection;

struct ThingyTable {
  typedef string TableEntry;
  typedef map<int, TableEntry> RawTable;

  ThingyTable() { }
  ThingyTable(string filename) {
    ifstream ifs(filename.c_str(), ios_base::binary);
    
    while (ifs.good()) {
      string numberString;
      getline(ifs, numberString, '=');
      if (!ifs.good()) {
        break;
      }
      
      // add hex ID for int conversion
      numberString = string("0x") + numberString;
      int rawID = TStringConversion::stringToInt(numberString);
      
      string charString;
      getline(ifs, charString);
      entries[rawID] = TableEntry(charString);
    }
  }
  
  string getEntry(int charID) {
    RawTable::iterator it = entries.find(charID);
    
    if (it == entries.end()) {
      return string("?");
    }
    
    return it->second;
  }
  
  RawTable entries;
};

struct SubstitutionFileEntry {
  
  string orig;
  string sub;
  
  void trySubstitute(string& str) {
    trySubstitutes(str, orig, sub);
    tryAltSubstitutes(str, orig, sub, ' ', '\n');
    tryAltSubstitutes(str, orig, sub, ' ', '.');
    tryAltSubstitutes(str, orig, sub, ' ', '\'');
//    tryAltSubstitutes(str, orig, sub, ' ', ',');
//    tryAltSubstitutes(str, orig, sub, ' ', '?');
//    tryAltSubstitutes(str, orig, sub, ' ', '!');
//    tryAltSubstitutes(str, orig, sub, ' ', '-');
  }
  
  void trySubstitutes(string& str, string origReal, string subReal) {
    int limit = str.size() - origReal.size();
    for (int i = 0; i <= limit; i++) {
      string substitute = str.substr(i, origReal.size());
      
      if (substitute.compare(origReal) == 0) {
        str = str.substr(0, i)
              + subReal
              + str.substr(i + subReal.size(),
                           str.size() - i + subReal.size());
      }
    }
  }
  
  void tryAltSubstitutes(string& str, string origReal, string subReal,
                         char orig, char sub) {
    for (int i = 0; i < origReal.size(); i++) {
      if (origReal[i] == orig) {
        string origSub = origReal.substr(0, i)
                         + sub;
        if (i < origReal.size() - 1) {
          origSub += origReal.substr(i + 1, origReal.size() - (i));
        }
        
        string subSub = subReal.substr(0, i)
                         + sub;
        if (i < subReal.size() - 1) {
          subSub += subReal.substr(i + 1, subReal.size() - (i));
        }
      
        trySubstitutes(str, origSub, subSub);
      }
    }
  }
  
  string substituteChars(string& origStr,
                      char orig, char sub) {
    string dst;
    for (int i = 0; i < origStr.size(); i++) {
      if (origStr[i] == orig) {
        dst += sub;
      }
      else {
        dst += origStr[i];
      }
    }
    
    return dst;
  }
  
};

struct SubstitutionFile {
  
  typedef vector<SubstitutionFileEntry> SubstitutionFileEntryCollection;
  
  void load(istream& ifs) {
    entries.clear();
  
    while (ifs.good()) {
      SubstitutionFileEntry entry;
      
      string next;
      getline(ifs, next);
      
      if (!ifs.good()) {
        break;
      }
      
      if (next.size() <= 0) {
        continue;
      }
      
      // comments
      if (next[0] == '#') {
        continue;
      }
      
      // skip blank lines
      bool valid = false;
      for (int i = 0; i < next.size(); i++) {
        if (!(isspace(next[i]))) {
          valid = true;
          break;
        }
      }
      
      if (!valid) {
        continue;
      }
      
      int origStart = 0;
      int origEnd = 0;
      
      int subStart = 0;
      int subEnd = 0;
      
      for (int i = 0; i < next.size(); i++) {
        if (next[i] == '|') {
          origEnd = i - 1;
          break;
        }
      }
      
      subStart = origEnd + 3;
      for (int i = subStart; i < next.size(); i++) {
        if (next[i] == ';') {
          subEnd = i;
          break;
        }
      }
      
      entry.orig = next.substr(origStart, origEnd - origStart);
      entry.sub = next.substr(subStart, subEnd - subStart);
      
      entry.orig = " " + entry.orig;
      entry.sub = " " + entry.sub;
      
      entries.push_back(entry);
    }
  }
  
  void doSubstitutions(string& str) {
    for (int i = 0; i < entries.size(); i++) {
      entries[i].trySubstitute(str);
    }
  }
  
  SubstitutionFileEntryCollection entries;
};

struct RawChunk {
  
  
  int address;
  RawData chunkData;
};
  
struct ScriptChunks {
  typedef vector<RawChunk> RawChunkCollection;
  
  void load(TByte* src, int& pos, int fileSize) {
    
    int startAddr = pos;
    int endAddr = startAddr
      + ByteConversion::fromBytes(src + pos,
                                  ByteSizes::uint32Size,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    
    int numEntries = (endAddr - pos) / ByteSizes::uint32Size;
    chunks.resize(numEntries);
    for (int i = 0; i < numEntries; i++) {
      int offset = TSerialize::readInt(src, pos,
                                              ByteSizes::uint32Size,
                                              EndiannessTypes::big,
                                              SignednessTypes::nosign);
      if (offset == 0xFFFFFFFF) {
        chunks[i].address = offset;
      }
      else {
        chunks[i].address = startAddr + offset;
      }
    }
    
    for (int i = 0; i < numEntries; i++) {
      if (chunks[i].address == 0xFFFFFFFF) {
        
      }
      else {
        // last entry should terminate at EOF
        int chunkLength = findNextChunkAddress(i, fileSize)
                              - chunks[i].address;
        
        chunks[i].chunkData = RawData((char*)src + chunks[i].address,
                                      chunkLength);
      }
    }
  }
  
  int findNextChunkAddress(int index, int fileSize) {
    for (int i = index + 1; i < chunks.size(); i++) {
      if (chunks[i].address != 0xFFFFFFFF) {
        return chunks[i].address;
      }
    }
    
    return fileSize;
  }
  
  RawChunkCollection chunks;
};

struct ScriptMessage {
  
  int address;
  int length;
  string data;
  
};

struct ScriptMessageFile {
  
  typedef vector<ScriptMessage> ScriptMessageCollection;
  
  enum MessageType {
    messageNone = 0,
    messageAttributed,
    messageUnattributed,
    messagePointedString,
    messageUnknown1,
    messageUnknown2
  };
  
  ScriptMessageFile() { }
  ScriptMessageFile(ThingyTable& thingyTable__)
    : thingyTable(thingyTable__) { }
  
  void generate(ScriptChunks& scriptChunks) {
    scriptMessages.clear();
  
    for (int i = 1; i < scriptChunks.chunks.size() - 1; i += 5) {
      if (looksLikeScriptIndex(scriptChunks.chunks[i],
                               scriptChunks.chunks[i + 1])) {
        addScriptMessages(scriptChunks.chunks[i],
                          scriptChunks.chunks[i + 1]);
      }
    }
    
    substituteMessages();
  }
  
  bool looksLikeScriptIndex(RawChunk& chunk, RawChunk& nextChunk) {
    int pos = 0;
    while (pos < chunk.chunkData.size()) {
      int offset = getNextIndexOffset(chunk, pos);
      
      // if offset is not in next chunk, this isn't an index
      if ((offset != 0xFFFF)
            && ((offset < 0)
                || (offset >= nextChunk.chunkData.size()))) {
        return false;
      }
    }
    
    return true;
  }
  
  void addScriptMessages(RawChunk& index, RawChunk& scripts) {
  
    int pos = 0;
    while (pos < index.chunkData.size()) {
      int offset = getNextIndexOffset(index, pos);
      
      if (offset == 0xFFFF) {
        continue;
      }
      
      int endOffset;
      if (pos >= index.chunkData.size() - 4) {
        endOffset = scripts.chunkData.size();
      }
      else {
        endOffset = getNextIndexOffset(index, pos);
        pos -= 4;
      }
      
      // we need a certain amount of space just to hold the message
      // start/end opcodes
      endOffset -= 3;
      
      int scriptPos = offset;
      RawData& script = scripts.chunkData;
      while (scriptPos < endOffset) {
      
        MessageType messageType
          = checkMessageStart(script, scriptPos);
          
        // special case: if the very first byte of the script is 0x20,
        // assume this is a string pointer
        if ((scriptPos == offset)
            && ((script[scriptPos] == 0x20)
                || (script[scriptPos] == 0x21)
                || (script[scriptPos] == 0x22))) {
          messageType = messagePointedString;
        }
          
        switch (messageType) {
        case messageAttributed:
          addAttributedMessage(script, scriptPos, scripts.address);
          break;
        case messageUnattributed:
          addUnattributedMessage(script, scriptPos, scripts.address);
          break;
        case messagePointedString:
          addPointedStringMessage(script, scriptPos, scripts.address);
          break;
        case messageUnknown1:
          addUnknown1Message(script, scriptPos, scripts.address);
          break;
        case messageUnknown2:
          addUnknown2Message(script, scriptPos, scripts.address);
          break;
        default:
          ++scriptPos;
          break;
        }
        
      }
    }
  }
  
  void substituteMessages() {
    for (int i = 0; i < scriptMessages.size(); i++) {
      substituteMessage(scriptMessages[i]);
    }
  }
  
  void substituteMessage(ScriptMessage& message) {
    
    TByte* str = (TByte*)(message.data.c_str());
    string newMessage;
    
    for (int i = 0; i < message.data.size(); i++) {
      if ((str[i] == 0x01)) {
        newMessage += "\n";
      }
      else if ((str[i] == 0x02)) {
        newMessage += "[BRK]\n\n";
      }
      // portrait attribution
      else if (((str[i] == 0x0C)
                || (str[i] == 0x0F))
                && (str[i + 1] == 0x00)) {
        // left side of box
        if (str[i] == 0x0C) {
          newMessage += "[POR";
        }
        // right side of box
        else {
          newMessage += "[ROR";
        }
        
        // two-digit decimal encoding
        newMessage += ((char)0x30 + ((str[i + 2] / 10) % 10));
        newMessage += ((char)0x30 + ((str[i + 2] / 1) % 10));
        
        newMessage += "]\n";
        
        // skip 3 characters
        i += 2;
        continue;
      }
      // 2b character
      else if ((str[i] >= 0x10) && (str[i] <= 0x15)) {
        int num = (str[i] & 0x0F) << 8;
        ++i;
        num |= (str[i] & 0xFF) << 0;
        newMessage += thingyTable.getEntry(num + 0xE0);
      }
      // unrecognized or invalid stuff
      else if (str[i] < 0x20) {
        newMessage += "[";
        newMessage += TStringConversion::intToString(
                          str[i], TStringConversion::baseHex);
        newMessage += "]";
      }
      // 1b character
      else {
        newMessage += thingyTable.getEntry(str[i] - 0x20);
      }
    }
      
    message.data = newMessage;
  }
  
  MessageType checkMessageStart(RawData& script, int scriptPos) {
    TByte* data = (TByte*)(script.c_str()) + scriptPos;
  
    // attributed message
    if (((data[0] == 0x0C)
          || (data[0] == 0x0F))
        && (data[1] == 0x00)) {
      return messageAttributed;
    }
    // unattributed message
    else if ((data[0] == 0xF0)
        && (data[1] == 0x00)) {
      return messageUnattributed;
    }
    else if (((data[0] == 0x30)
              || (data[0] == 0x31)
              || (data[0] == 0x32)
              || (data[0] == 0x33)
              || (data[0] == 0x34)
              || (data[0] == 0x35)
              || (data[0] == 0x36))
        && ((data[2] == 0x20)
              || (data[2] == 0x21)
              || (data[2] == 0x22)
              || (data[2] == 0x23)
              || (data[2] == 0x24))) {
      return messageUnknown1;
    }
    else if ((data[0] == 0x00)
        && (data[1] == 0x00)
        && (data[2] == 0x20)) {
      return messageUnknown1;
    }
    // ???????????????????????????
    // M2F 'YOU MUST BE ONE'
    else if ((data[0] == 0x05)
        && (data[1] == 0x00)
        && (data[2] == 0x21)) {
      return messageUnknown1;
    }
    // M44, epilogue 1: "DEAR HIRO,"
    else if ((data[0] == 0x10)
        && (data[1] == 0x00)
        && (data[2] == 0x23)) {
      return messageUnknown1;
    }
    else if ((data[0] == 0x20)
        && (data[2] == 0x0B)) {
      return messageUnknown2;
    }
    // if a "message end" command is directly followed
    // by a string, it's a new message?
    else if ((scriptPos > 1)
        && ((data[0] == 0x20)
                || (data[0] == 0x21))
        && ((data[-1] == 0x07)
            || (data[-2] == 0x07))) {
      return messagePointedString;
    }
    // M17: HA HA HA HA!
    // ARE YOU THE ONE SEEKING...
    else if ((scriptPos > 2)
        && (data[0] == 0x38)
        && (data[-1] == 0x00)
        && (data[-2] == 0x10)) {
      return messageUnknown1;
    }
    // if length leads to an "end", message
    else if (((data[0] == 0x20)
                || (data[0] == 0x21))
             && ((scriptPos + data[1]) < script.size())
             && ((data[data[1]] == 0x07)
                 || (data[data[1] + 1] == 0x07))) {
        return messagePointedString;
    }
    // item icon display?
    else if ((data[0] == 0x0B)
        && (data[1] == 0x00)) {
      return messageUnattributed;
    }
    
    return messageNone;
  }
  
  void addAttributedMessage(RawData& script, int& scriptPos,
                               int scriptRealAddress) {
    addBasicStringMessage(script, scriptPos, scriptRealAddress);
  }
  
  void addUnattributedMessage(RawData& script, int& scriptPos,
                               int scriptRealAddress) {
    scriptPos += 3;

    // this happens in M1F: WITH ALL THE TRAVELIN'...
    if (script[scriptPos - 1] == 0x20) {
      ++scriptPos;
    }
    
    addBasicStringMessage(script, scriptPos, scriptRealAddress);
  }
  
  void addPointedStringMessage(RawData& script, int& scriptPos,
                               int scriptRealAddress) {
    // second byte is probably length; we're skipping it for now
    scriptPos += 2;
    
    addBasicStringMessage(script, scriptPos, scriptRealAddress);
  }
  
  void addUnknown1Message(RawData& script, int& scriptPos,
                               int scriptRealAddress) {
    scriptPos += 4;
    
    addBasicStringMessage(script, scriptPos, scriptRealAddress);
  }
  
  void addUnknown2Message(RawData& script, int& scriptPos,
                               int scriptRealAddress) {
    scriptPos += 5;
    
    addBasicStringMessage(script, scriptPos, scriptRealAddress);
  }
  
  void addBasicStringMessage(RawData& script, int& scriptPos,
                               int scriptRealAddress) {
    int startPos = scriptPos;
    TByte* data = (TByte*)(script.c_str());
    while (true) {
      // screen for Lemina's portrait code (0C 00 07) and other
      // subcodes that interfere with end-of-message detection
      if ((script[scriptPos] == 0x07)
          && ((script[scriptPos - 2] != 0x0C)
              && (script[scriptPos - 2] != 0x0B)
              && (script[scriptPos - 2] != 0x0F))) {
        break;
      }
      
      // portrait code
      if ((script[scriptPos] == 0x0C)
          || (script[scriptPos] == 0x0B)
          || (script[scriptPos] == 0x0F)) {
        scriptPos += 2;
      }
      
      // two-byte kanji IDs
      if ((script[scriptPos] >= 0x10)
          && (script[scriptPos] <= 0x15)) {
        ++scriptPos;
      }
      
      ++scriptPos;
      
      // heuristically screen misdetected messages
      // 0 = bad
      if ((script[scriptPos] == 0x00)
          || ((script[scriptPos] <= 0x1F)
              && (script[scriptPos] > 0x15))) {
        scriptPos = startPos + 1;
        return;
      }
    }
    
    ScriptMessage message;
    message.address = scriptRealAddress + startPos;
    message.data = script.substr(startPos, scriptPos - startPos);
    message.length = scriptPos - startPos;
    scriptMessages.push_back(message);
    
    // skip the terminating code
    ++scriptPos;
  }
  
  int getNextIndexAddress(RawChunk& chunk, int& pos) {
      // skip unknown field
      pos += 2;
      // check offset
      int offset = getNextIndexOffset(chunk, pos);
      
      return offset + chunk.address;
  }
  
  int getNextIndexOffset(RawChunk& chunk, int& pos) {
      // skip unknown field
      pos += 2;
      return TSerialize::readInt(chunk.chunkData.c_str(),
                                      pos,
                                      ByteSizes::uint16Size,
                                      EndiannessTypes::big,
                                      SignednessTypes::nosign);
  }
  
  ScriptMessageCollection scriptMessages;
  ThingyTable thingyTable;
};

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

int main(int argc, char* argv[]) {
  
  if (argc < 3) {
    cout <<
    "scriptrip: Lunar: Eternal Blue (Mega CD) Japanese dialogue extractor\n"
    "Usage: scriptrip_jp <map> <thingy> [subs]\n"
    "<map> is a Lunar Eternal Blue map file (e.g. M01.GRP).\n"
    "<thingy> is a Thingy table used to convert the Japanese characters.\n"
    "[subs] is an optional text file specifying strings to replace in"
    " the output.\n";
    return 0;
  }
  
  string filename = string(argv[1]);
  
  ifstream ifs(filename.c_str(), ios_base::binary);
  int fileSize = fsize(ifs);
  TByte* buffer = new TByte[fileSize];
  ifs.read((char*)buffer, fileSize);
  
  int mapObjectBlockAddress
    = ByteConversion::fromBytes(buffer + mapObjectBlockPointerOffset,
                                ByteSizes::uint32Size,
                                EndiannessTypes::big,
                                SignednessTypes::nosign);
                                
  ThingyTable thingyTable = ThingyTable(string(argv[2]));
  
  ScriptChunks scriptChunks;
  int pos = mapObjectBlockAddress;
  scriptChunks.load(buffer, pos, fileSize);
  
  ScriptMessageFile scriptMessageFile = ScriptMessageFile(thingyTable);
  scriptMessageFile.generate(scriptChunks);
  
  cout << "File: " << filename << endl << endl;
  cout << "Messages: " << scriptMessageFile.scriptMessages.size()
       << endl << endl;
  
  for (int i = 0; i < scriptMessageFile.scriptMessages.size(); i++) {
    cout << "=== " << hex
                  << i
         << " | " << scriptMessageFile.scriptMessages[i].address
         << " | " << scriptMessageFile.scriptMessages[i].length
         << " ===" << endl;
    cout << scriptMessageFile.scriptMessages[i].data;
    cout << "[END]";
    cout << endl << endl;
  }
  
  delete buffer;
  
  return 0;
} 
