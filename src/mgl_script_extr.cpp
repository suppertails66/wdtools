#include "mgl_cmpr.h"
#include "util/ByteConversion.h"
#include "util/TArray.h"
#include "util/TSerialize.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>

using namespace std;
using namespace BlackT;

const static int indexSize = 0x800;

struct CharacterExpression {
  char* name;
  char* expression;
};

CharacterExpression portraitMappings[] = {
  // 0 -- reserved for "no portrait"
  { NULL, NULL },
  // 1
  { "Elie", "neutral" },
  { "Elie", "serious" },
  { "Elie", "sad" },
  { "Elie (v2)", "neutral" },
  { "Elie (v2)", "serious" },
  { "Lena", "neutral" },
  { "Lena", "serious" },
  { "Lena", "happy" },
  { "Senia", "neutral" },
  { "Senia", "serious" },
  // 11
  { "Senia", "happy" },
  { "Blade", "neutral" },
  { "Blade", "serious" },
  { "Wynn", "neutral" },
  { "Wynn", "serious" },
  { "Wynn", "happy" },
  { "Wynn (possessed)", NULL },
  { "Azu", NULL },
  { "Glen", NULL },
  { "Ant", "neutral" },
  // 21
  { "Ant", "angry" },
  { "Layla", NULL },
  { "Steel", NULL },
  { "Dadis", NULL },
  { "Terena", NULL },
  { "Emma", NULL },
  { "Eleonora", NULL },
  { "Brown", NULL },
  { "Kule", NULL },
  { "Rick", NULL },
  // 31
  { "Shiela", NULL },
  { "D", NULL },
  { "Barua", NULL },
  { "Memphis", NULL },
  { "Hyde", NULL },
  { "Richter", NULL },
  { "Kent", NULL },
  { "Brune", NULL },
  { "Stella", NULL },
  { "Iason", NULL },
  // 41
  { "Ralph", NULL },
  { "Blue Dragon", NULL },
  { "Elie (v2)", "happy" },
  { "Elie (v2)", "sad" }
};

struct MglScriptHeader {
  const static int size = 8;

  int id;
  int offset;
  
  void read(unsigned char* src) {
    id = ByteConversion::fromBytes(
      src + 0, 4,
      EndiannessTypes::big, SignednessTypes::nosign);
    offset = ByteConversion::fromBytes(
      src + 4, 4,
      EndiannessTypes::big, SignednessTypes::nosign);
  }
};

bool morePrintableContentExists(unsigned char* str) {
  while (true) {
    if (*str == 0x00) return false;
    else if (*str < 0x20) ++str;
    else return true;
  }
}

struct ScriptChunk {
  int unknown1;
  int unknown2;
  vector<MglScriptHeader> scriptHeaders;
  TArray<unsigned char> scriptData;
  vector<string> dialogues;
  vector<int> dialogueOffsets;
  
  void read(unsigned char* src, int srcsize) {
    int scriptInfoOffset = ByteConversion::fromBytes(src + 4, 4,
      EndiannessTypes::big, SignednessTypes::nosign);
    
    int numDialogues = ByteConversion::fromBytes(
      src + scriptInfoOffset + 0, 4,
      EndiannessTypes::big, SignednessTypes::nosign);
    int dialoguesOffset = ByteConversion::fromBytes(
      src + scriptInfoOffset + 4, 4,
      EndiannessTypes::big, SignednessTypes::nosign);
    int numScripts = ByteConversion::fromBytes(
      src + scriptInfoOffset + 8, 4,
      EndiannessTypes::big, SignednessTypes::nosign);
    
    scriptHeaders.clear();
    for (int i = 0; i < numScripts; i++) {
      MglScriptHeader header;
      header.read(src + scriptInfoOffset + 12 + (i * MglScriptHeader::size));
      
      scriptHeaders.push_back(header);
    }
    
    // scripts are located starting at the offset of the first script
    // and ending at the start of the first dialogue chunk
    if ((numScripts > 0) && (numDialogues > 0)) {
      int scriptStartOffset = scriptHeaders[0].offset;
      int scriptEndOffset = ByteConversion::fromBytes(
        src + dialoguesOffset + 0, 4,
        EndiannessTypes::big, SignednessTypes::nosign);
      int scriptSize = scriptEndOffset - scriptStartOffset;
      
//      cerr << scriptStartOffset << " " << scriptEndOffset << endl;
      
      scriptData.resize(scriptSize);
      memcpy(scriptData.data(), src + scriptStartOffset, scriptSize);
    }
    
    dialogueOffsets.clear();
    for (int i = 0; i < numDialogues; i++) {
      int dialogueOffset = ByteConversion::fromBytes(
        src + dialoguesOffset + (i * 4), 4,
        EndiannessTypes::big, SignednessTypes::nosign);
      dialogueOffsets.push_back(dialogueOffset);
      
      int nextOffset;
      if (i < (numDialogues - 1)) {
        nextOffset = ByteConversion::fromBytes(
          src + dialoguesOffset + ((i + 1) * 4), 4,
          EndiannessTypes::big, SignednessTypes::nosign);
      }
      else {
        nextOffset = srcsize;
      }
      
//      cerr << hex << dialogueOffset << endl;
 /*     int endpos = dialogueOffset;
      while (true) {
        // dialogue is terminated by the sequence 08 00
        if ((src[endpos] == 0x08) && (src[endpos + 1] <= 0x0A)) break;
        // or just 00???
        if ((src[endpos] == 0x00)) break;
        // or if we run out of room
        else if (endpos > srcsize - 2) break;
        else ++endpos;
      } */
      
//      int endpos = nextOffset - 2;
//      if (endpos < dialogueOffset) endpos = dialogueOffset;
      
/*      for (int i = dialogueOffset; i < endpos - 1; i++) {
        if ((src[i] == 0x00) && (src[i + 1] == 0x00)) {
          endpos = i;
          break;
        }
      } */
      
      string dialogue;
      
      int endcheck = dialogueOffset;
//      int endpos = dialogueOffset;
      bool endedWith08 = false;
      while (endcheck < nextOffset) {
        unsigned char next = src[endcheck];
        
        // 2-byte SJIS sequences
        if (next >= 0x80) {
          dialogue += (char)next;
          dialogue += (char)(src[endcheck + 1]);
          
          endcheck += 2;
          
          endedWith08 = false;
          
          continue;
        }
        
        // 0x08 = pause for player input
        if (next == 0x08) {
          // add double linebreak *only* if more text follows
          if (morePrintableContentExists(src + endcheck + 1)) {
//          if (src[endcheck + 1] != 0x00) {
            dialogue += "\n\n";
          }
          ++endcheck;
          
          endedWith08 = true;
          
          continue;
        }
        
        // 0x0C = clear text box, continue printing
        if (next == 0x0C) {
          // this is sometimes misused at the beginning or end of a piece
          // of dialogue, where it has no effect
          // we ignore it in those cases to avoid clutter
          if ((endcheck != dialogueOffset)
              && morePrintableContentExists(src + endcheck + 1)) {
            dialogue += "\\c";
          }
          ++endcheck;
          continue;
        }
        
        // 0x0D = printing delay?
        if (next == 0x0D) {
          dialogue += "\\p";
          ++endcheck;
          continue;
        }
      
        // 00 = terminator
        if ((src[endcheck] == 0x00)) {
//          endpos = endcheck;
          break;
        }
        
        // anything else: add to string
        dialogue += next;
        ++endcheck;
        endedWith08 = false;
      }
      
      // If the dialogue window was *not* closed with a 0x08 command,
      // mark it as such; otherwise, this is implicit to avoid needless
      // clutter
      if (!endedWith08) {
//      if ((src[endcheck - 1] != 0x08)) {
        dialogue += "[NOWAIT]";
      }
//      if ((src[endcheck - 1] != 0x08)
//          && (src[endcheck - 2] != 0x08)) {
//        dialogue += "[NOCLOSE]";
//      }
      
//      string dialogue((char*)src + dialogueOffset, endpos - dialogueOffset);
      dialogues.push_back(dialogue);
    }
  }
};

void doChunk(ScriptChunk& chunk, int chunkIndex) {
  
  // no dialogue or scripts means we don't care about this chunk
  if ((chunk.dialogues.size() <= 0) || (chunk.scriptHeaders.size() <= 0)) {
    return;
  }
  
  map<int, bool> usedDialogues;
  map<int, int> dialoguePortraits;
  
  int scriptpos = 0;
  int portraitnum = 0;
  while (scriptpos < chunk.scriptData.size()) {
    switch (chunk.scriptData[scriptpos]) {
    // change portrait
    case 0x16:
      {
        int op1 = chunk.scriptData[scriptpos + 1];
        int op2 = chunk.scriptData[scriptpos + 2];
        
        // sanity checks
        
        // position must be 0 or 1
        if ((op1 != 0) && (op1 != 1)) break;
        // valid portrait numbers are 0 to 2C
        // (there are 2C portrait images; index zero is reserved for "no
        // portrait")
        if ((op2 > 0x2C)) break;
        
        portraitnum = op2;
        
        scriptpos += 3;
        continue;
      }
      break;
    // print message
    case 0x17:
      {
        int op1 = chunk.scriptData[scriptpos + 1];
        int op2 = chunk.scriptData[scriptpos + 2];
        
        // sanity checks
        
        // position must be 0 or 1
//        if ((op1 != 0) && (op1 != 1)) break;
        // dialogue number must be in valid range
        if ((op2 >= chunk.dialogues.size())) break;
        // no using dialogue more than once
        if (usedDialogues.find(op2) != usedDialogues.end()) {
          cerr << "Possible dialogue reuse: " << usedDialogues[op2] << endl;
          break;
        }
        
//        cout << portraitnum << " " << chunk.dialogues[op2] << endl;
        dialoguePortraits[op2] = portraitnum;
        
        usedDialogues[op2] = true;
        
        scriptpos += 3;
        continue;
      }
      break;
    }
    
    ++scriptpos;
  }
  
  for (int i = 0; i < chunk.dialogues.size(); i++) {
    if (!usedDialogues[i]) {
      cerr << "Dialogue " << i << " not used: "
        << chunk.dialogues[i] << endl;
    }
  }
  
  // print all dialogue, with portrait if possible
  for (int i = 0; i < chunk.dialogues.size(); i++) {
    cout << dec << chunkIndex << "," << "0x" << hex << chunk.dialogueOffsets[i]
      << ",";
    
    map<int, int>::iterator findIt = dialoguePortraits.find(i);
    if (findIt != dialoguePortraits.end()) {
      // no portrait
      if (findIt->second == 0) {
//        cout << dec << findIt->second << "," << findIt->second << ",";
        cout << ",,";
      }
      else {
//        cout << dec << findIt->second << "," << findIt->second << ",";
        cout << portraitMappings[findIt->second].name << ",";
        if (portraitMappings[findIt->second].expression != NULL) {
          cout << portraitMappings[findIt->second].expression << ",";
        }
        else {
          cout << ",";
        }
      }
    }
    // portrait not detected for this line
    else {
      cout << "?,?,";
    }
    
    cout << "\"" << chunk.dialogues[i] << "\"";
    
    cout << "," << endl;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Mahou Gakuen Lunar! script/dialogue extraction tool\n";
    cout << "Usage: " << argv[0] << " <dialoguefile>" << endl;
  }
  
/*  ifstream ifs(argv[1], ios_base::binary);
  
  int insize = fsize(ifs);
  if (insize <= 0) return 0;
  
  ifs.close();
  
  BufferedFile scriptfile = getFile(argv[1]);
  
  ScriptChunk chunk;
  chunk.read((unsigned char*)(scriptfile.buffer), scriptfile.size); */
  
  cout << "chunk,offset,character,expression,japanese,english" << endl;
  
  ifstream ifs(argv[1], ios_base::binary);
  char index[indexSize];
  ifs.read(index, indexSize);
  int chunkOffset, chunkSize;
  int indexpos = 0;
  while (true) {
    chunkOffset = ByteConversion::fromBytes(index + (indexpos * 8) + 0, 4,
      EndiannessTypes::big, SignednessTypes::sign);
    chunkSize = ByteConversion::fromBytes(index + (indexpos * 8) + 4, 4,
      EndiannessTypes::big, SignednessTypes::sign);
    
    ++indexpos;
    
    if (chunkOffset == -1) break;
    
    if (chunkOffset == 0) continue;
    
    unsigned char* rawChunk = new unsigned char[chunkSize];
    ifs.seekg(chunkOffset);
    ifs.read((char*)rawChunk, chunkSize);
    
    ScriptChunk chunk;
    chunk.read(rawChunk, chunkSize);
  
    doChunk(chunk, indexpos - 1);
    
    delete rawChunk;
  }
  
//  cout << chunk.dialogues.size() << endl;
  
  return 0;
} 
