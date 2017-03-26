#include "scriptrip_tss_utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "util/TByte.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"

using namespace std;
using namespace BlackT;
  
  string orig;
  string sub;
  
  void SubstitutionFileEntry::trySubstitute(string& str) {
    trySubstitutes(str, orig, sub);
    tryAltSubstitutes(str, orig, sub, ' ', '\n');
    tryAltSubstitutes(str, orig, sub, ' ', '.');
    tryAltSubstitutes(str, orig, sub, ' ', '\'');
//    tryAltSubstitutes(str, orig, sub, ' ', ',');
//    tryAltSubstitutes(str, orig, sub, ' ', '?');
//    tryAltSubstitutes(str, orig, sub, ' ', '!');
//    tryAltSubstitutes(str, orig, sub, ' ', '-');
  }
  
  void SubstitutionFileEntry::trySubstitutes(string& str, string origReal, string subReal) {
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
  
  void SubstitutionFileEntry::tryAltSubstitutes(string& str, string origReal, string subReal,
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
  
  string SubstitutionFileEntry::substituteChars(string& origStr,
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
  
  void SubstitutionFile::load(istream& ifs) {
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
      
      entries.push_back(entry);
      
      entry.orig = " " + entry.orig;
      entry.sub = " " + entry.sub;
      
      entries.push_back(entry);
    }
  }
  
  void SubstitutionFile::doSubstitutions(string& str) {
    for (int i = 0; i < entries.size(); i++) {
      entries[i].trySubstitute(str);
    }
  }

  void ScriptChunks::load(TByte* src, int& pos, int fileSize) {
    
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
  
  int ScriptChunks::findNextChunkAddress(int index, int fileSize) {
    for (int i = index + 1; i < chunks.size(); i++) {
      if (chunks[i].address != 0xFFFFFFFF) {
        return chunks[i].address;
      }
    }
    
    return fileSize;
  }
  
//  void generate(ScriptChunks& scriptChunks) {
  void ScriptMessageFile::generate(TByte* src, int srcSize, int startAddress,
                    int threshold,
                    int messageTotalWeight,
                    int messageSizeWeight) {
    scriptMessages.clear();
    
    addScriptMessages(src, srcSize, startAddress,
                      threshold, messageTotalWeight, messageSizeWeight);
    
    substituteMessages();
  }
  
  void ScriptMessageFile::addScriptMessages(
                    TByte* src, int srcEnd,
                    int startAddress,
                    int threshold,
                    int messageTotalWeight,
                    int messageSizeWeight) {
  
    int pos = startAddress;
    while (pos < (srcEnd - 1)) {
    
      // Strategy 1 has had mixed results, so let's try something else.
      //
      // Test each and every position in the chunk to see if it's a string table.
      // Compute a "probability factor" based on a combination of number of
      // strings read and average length of each string.
      // If the probability factor is high enough, assume this is a real string
      // table.
      
      int simPos = pos;
//      cerr << "pos1: " << pos << endl;
      int mLen;
      ScriptMessageCollection messages;
      while (mLen = addBasicStringMessage(src, srcEnd, simPos, messages)) {
        simPos += mLen;
      }
      
      if (messages.size() == 0) {
        ++pos;
//        cerr << "1: " << pos << endl;
        continue;
      }
      
      // Compute probability factor
      int probability = messages.size() * messageTotalWeight;
      double averageMessageSize = 0;
      for (int i = 0; i < messages.size(); i++) {
        averageMessageSize += messages[i].length;
      }
      averageMessageSize /= messages.size();
      probability += (averageMessageSize * messageSizeWeight);
      
//      cerr << probability << endl;
//      cerr << "pos2: " << pos << endl;
      
      if (probability >= threshold) {
//        cerr << messages.size();
//        cerr << messages[0].data << endl;
//        cerr << addBasicStringMessage(src, srcEnd, pos) << endl;
        while (mLen = addBasicStringMessage(src, srcEnd, pos)) {
          pos += mLen;
//          cerr << "3: " << pos << endl;
        }
      }
      else {
        ++pos;
//        cerr << "2: " << pos << endl;
      }
      
/*      // Maps alternate between chunks of raw 68000 code and unindexed tables
      // of dialogue strings. Our strategy is to start by searching for an
      // RTS opcode (4E 75), because we know that that will be used to
      // terminate the 68000 code block. Dialogue strings should begin
      // immediately afterwards. At that point, we repeatedly call
      // addBasicStringMessage() until it no longer detects dialogue strings,
      // then begin seeking for an RTS again.
      
      // The main complication here is detecting which RTS is in fact the
      // last one.
      
      // Scan for RTS (4E 75)
      while ((pos < (srcEnd - 18))
             && !((src[pos] == '\x4E')
                  && (src[pos + 1] == '\x75'))) {
        ++pos;
      }
      
      // done?
      if (pos >= (srcEnd - 17)) {
        break;
      }
      
      // Skip the RTS
      pos += 2;
      
      // Try to add strings (scanning up to 16 characters from start)
      int mLen;
      for (int i = 0; i < 16; i++) {
        mLen = addBasicStringMessage(src, srcEnd, pos + i);
        if (mLen != 0) break;
      }
      
      // misdetection: start over
      if (mLen == 0) {
        continue;
      }
      
      pos += mLen;
      
      // add sequential strings until we reach the end
      do {
        mLen = addBasicStringMessage(src, srcEnd, pos);
        pos += mLen;
        
        if (pos >= srcEnd - 16) break;
        
        if (mLen == 0) {
          // scan up to 16 chars for resume
          for (int i = 0; i < 16; i++) {
            mLen = addBasicStringMessage(src, srcEnd, pos + i);
            if (mLen != 0) break;
          }
          
          // end
          if (mLen == 0) {
            break;
          }
          
          pos += mLen;
        }
        
      } while (true);
//      } while ((pos < srcEnd) && (mLen > 0)); */
      
    }
  }
  
  void ScriptMessageFile::substituteMessages() {
    for (int i = 0; i < scriptMessages.size(); i++) {
      substituteMessage(scriptMessages[i]);
    }
  }
  
  void ScriptMessageFile::substituteMessage(ScriptMessage& message) {
    TByte* str = (TByte*)(message.data.c_str());
    string newMessage;
    
    bool caps = true;
    char lastPunct = 0x00;
    int ellipseCount = 0;
    bool lastWasPeriod = false;
    for (int i = 0; i < message.data.size(); i++) {
    
      if ((str[i] == '.')
          || (str[i] == '!')
          || (str[i] == '?')) {
        caps = true;
      }
      
      // don't capitalize after ellipses
      if ((i >= 2)
          && (str[i] == '.')
          && (str[i - 1] == '.')
          && (str[i - 2] == '.')) {
        caps = false;
      }
      
      if (ispunct(str[i])) {
        lastPunct = str[i];
      }
    
      if ((str[i] == 0x01)) {
        newMessage += "\n";
      }
      else if ((str[i] == 0x02)) {
        newMessage += "[BRK]\n\n";
        
        // if parts of a message are separated across boxes with
        // a comma, don't capitalize start of next box
        if ((lastPunct != ',')) {
          caps = true;
        }
      }
      // portrait attribution
      else if ((str[i] == 0x04)) {
        // left side of box
//        if (str[i] == 0x0C) {
          newMessage += "[POR";
//        }
        // right side of box
//        else {
//          newMessage += "[ROR";
//        }
        
        // three-digit decimal encoding
        newMessage += ((char)0x30 + ((str[i + 1] / 100) % 10));
        newMessage += ((char)0x30 + ((str[i + 1] / 10) % 10));
        newMessage += ((char)0x30 + ((str[i + 1] / 1) % 10));
        
        newMessage += "]\n";
        
        // skip 2 characters
        i += 1;
        continue;
      }
      // special
      else if ((i <= message.data.size() - 1) && (str[i] == 0x09)) {
        newMessage += "[";
        newMessage += TStringConversion::intToString(
                          str[i], TStringConversion::baseHex);
        newMessage += "]";
        newMessage += "[";
        newMessage += TStringConversion::intToString(
                          str[i + 1], TStringConversion::baseHex);
        newMessage += "]";
        
        // skip 2 characters
        i += 1;
      }
      else if ((str[i] < 0x20)
          || (str[i] >= 0x7F)) {
        newMessage += "[";
        newMessage += TStringConversion::intToString(
                          str[i], TStringConversion::baseHex);
        newMessage += "]";
      }
      else if (str[i] == '[') {
        newMessage += "\\[";
      }
      else if (str[i] == ']') {
        newMessage += "\\]";
      }
      else if (str[i] == '\\') {
        newMessage += "\\\\";
      }
      else {
        // very short messages are likely to be misdetections, so we leave
        // them as-is to avoid crashes
//        if (changeCase && (message.data.size() > 5)) {
        if (changeCase) {
          if (caps && isalpha(str[i])) {
            newMessage += toupper(str[i]);
            caps = false;
          }
          else {
            newMessage += tolower(str[i]);
            
            // annoying
            if ((i > 0)
                && (i < message.data.size() - 1)
                && (str[i] == 'I')
                && (str[i - 1] == ' ')
                && (str[i + 1] == 0x01)) {
              newMessage[newMessage.size() - 1] = 'I';
            }
          }
        }
        else {
          newMessage += str[i];
        }
      }
    }
    
    message.data = newMessage;
  }
  
  int ScriptMessageFile::addBasicStringMessage(TByte* realSrc, int srcEnd,
                            int messageRealAddress) {
    return addBasicStringMessage(realSrc, srcEnd, messageRealAddress,
                          scriptMessages);
  }
  
  int ScriptMessageFile::addBasicStringMessage(TByte* realSrc, int srcEnd,
                            int messageRealAddress,
                            ScriptMessageCollection& dst) {
    TByte* src = realSrc + messageRealAddress;
    int pos = 0;
    while (true) {
      if ((pos == 0) & (src[pos] == 0x00)) {
        return 0;
      }
    
      // 00 = terminator, but screen for portrait codes
      if ((pos > 0)
          && (src[pos] == 0x00)
          && ((src[pos - 1] != 0x04))) {
        break;
      }

      // screen for portrait codes
      if (src[pos] == 0x04) {
//        cerr << messageRealAddress << endl;
        pos += 2;
        continue;
      }

      // screen for item pickup
      if (src[pos] == 0x09) {
//        cerr << messageRealAddress << endl;
        pos += 2;
        continue;
      }
      
      // heuristically screen misdetected messages
      // @ sign or out of ASCII range = bad
      if ((src[pos] == '@')
          || (src[pos] == '*')
          || (src[pos] == '_')
//          || (src[pos] == '<')
//          || (src[pos] == '>')
          || (src[pos] == '^')
//          || (src[pos] == '#')
          || (src[pos] == '$')) {
      
        return 0;
      }
             // 04 80 = close portrait window
      if ((((unsigned int)(src[pos]) >= 0x82)
               && ((pos == 0)
                   || ((pos > 0)
                       && (src[pos - 1] != 0x04))))) {
        return 0;
      }
      
      if (((src[pos] >= 0x6)
            && (src[pos] != 0x09)
            && (src[pos] < 0x20))) {
        return 0;
      }
      
      ++pos;
    }
    
    // null string = invalid
    if (pos == 0) {
      return 0;
    }
    
    // strings of length 3 or less = invalid
    if (pos <= 3) {
      return 0;
    }
    
    // alternate version for battle strings
/*    // strings of length 3 or less = invalid
    if (pos < 3) {
      return 0;
    }
    
    bool cap = false;
    for (int i = 0; i < pos; i++) {
      if ((src[i] >= 'A') && (src[i] <= 'Z')) {
        cap = true;
        break;
      }
    }
    if (!cap) return 0; */
    
    // if message contains 4E 75, it is VERY VERY SUSPECT
    for (int i = 0; i < pos - 1; i++) {
      if ((src[i] == 0x4E)
          && (src[i + 1] == 0x75)) {
        return 0;
      }
    }
    
    // if message contains 75, it is VERY VERY SUSPECT
    for (int i = 0; i < pos; i++) {
      if (src[i] == '\x75') {
        return 0;
      }
    }
    
    ScriptMessage message;
    message.address = messageRealAddress;
    message.data = string((char*)realSrc + messageRealAddress, pos);
    message.length = pos;
    dst.push_back(message);
    
    // skip the terminating code
    ++pos;
    
    return pos;
  }
  
  int ScriptMessageFile::getNextIndexAddress(RawChunk& chunk, int& pos) {
      // skip unknown field
      pos += 2;
      // check offset
      int offset = getNextIndexOffset(chunk, pos);
      
      return offset + chunk.address;
  }
  
  void ScriptMessageFile::print(string filename) {
    cout << "File: " << filename << endl << endl;
    cout << "Messages: " << scriptMessages.size()
         << endl << endl;
         
    for (int i = 0; i < scriptMessages.size(); i++) {
      cout << "=== " << hex
                    << i
           << " | " << scriptMessages[i].address
           << " | " << scriptMessages[i].length
           << " ===" << endl;
      cout << scriptMessages[i].data;
      cout << "[END]";
      cout << endl << endl;
    }
  }
  
  int ScriptMessageFile::getNextIndexOffset(RawChunk& chunk, int& pos) {
      // skip unknown field
      pos += 2;
      return TSerialize::readInt(chunk.chunkData.c_str(),
                                      pos,
                                      ByteSizes::uint16Size,
                                      EndiannessTypes::big,
                                      SignednessTypes::nosign);
  }

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}
