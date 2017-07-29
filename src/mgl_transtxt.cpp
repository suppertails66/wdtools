#include "mgl_transtxt.h"
#include "util/TSerialize.h"
#include "util/TStringConversion.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cctype>

using namespace std;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

std::string nextToken(std::istream& ifs) {
  string str;
  ifs >> str;
  
  while (ifs.good() && str.size() && (str[0] == '#')) {
    getline(ifs, str);
    ifs >> str;
  }
  
  return str;
}

void escapeString(const std::string& src, std::string& dst) {
  // is this a half-width string?
  if (src.size() && 
      (((src[0] >= 161) && (src[0] <= 223))
        || (src[0] == 1)
        || (src[0] == 2))) {
    for (unsigned int i = 0; i < src.size(); i++) {
      unsigned char next = src[i];
      // katakana mode
      if ((next == 1)) {
        dst += "{k}";
      }
      // hiragana mode
      else if ((next == 2)) {
        dst += "{h}";
      }
      else {
        dst += next;
      }
    }
  }
  else {
    dst = src;
  }
  
/*    if ((next < 0x0A) || (next >= 0xF0)) {
      char buffer[2];
      sprintf(buffer, "%02x", next);
      dst += "\\x";
      dst += buffer[0];
      dst += buffer[1];
    }
    else {
      dst += next;
    }
  } */
}

void skipWhitespace(std::istream& ifs) {
  while (ifs.good() && isspace(ifs.peek())) ifs.get();
}

void advance(std::istream& ifs) {
  skipWhitespace(ifs);
  while (ifs.good() && (ifs.peek() == '#')) {
    string str;
    getline(ifs, str);
    skipWhitespace(ifs);
  }
}

void printCommented(std::ostream& ofs, std::string& str) {
  ofs << "# ";
  for (unsigned int i = 0; i < str.size(); i++) {
    ofs << str[i];
    
    if (str[i] == '\n') {
      ofs << "# ";
    }
  }
  ofs << endl;
}

void readStringLiteral(std::istream& ifs, std::string& str) {
  str = "";

  advance(ifs);
  
  // skip initial "<\n"
  string temp;
  getline(ifs, temp);
  
  while (true) {
    // check escape sequences
    //
    // BUG: this is used to read the original text, which consists of 16-bit
    // SJIS characters, but they're read byte-by-byte, causing the low
    // byte to sometimes be interpreted as a backslash
    // the only real consequence of this is that auto-computed lengths
    // may be shorter than the original, which causes no harm
    // ... except in the very bad case that the fake backslash is the last
    // character before the terminator, which would make everything go
    // to hell
    //
    // FIXME!
    if (ifs.peek() == '\\') {
      ifs.get();
      
      if (ifs.peek() == 'x') {
        ifs.get();
        
        // read 2-digit hex literal
        string value;
        value += ifs.get();
        value += ifs.get();
        istringstream iss;
        iss.str(value);
        int c = 0;
        iss >> hex >> c;
        str += (char)c;
        continue;
      }
      else {
        str += ifs.get();
        continue;
      }
    }
    
    char next = ifs.get();
    // terminator: "\n>"
    if ((next == '\n') && (ifs.peek() == '>')) break;
    
    str += next;
  }
  
  ifs.get();
}
  
void TranslationEntry::save(std::ostream& ofs) {
  ofs << "#############################" << endl;
  ofs << "### source file and offset" << endl;
  ofs << sourceFile << " " << hex << sourceFileOffset << endl;
  ofs << endl;
  
  ofs << "### pointers" << endl;
  ofs << pointers.size();
  for (unsigned int i = 0; i < pointers.size(); i++) {
    ofs << " " << pointers[i];
  }
  ofs << endl;
  ofs << endl;
  
  ofs << "### size" << endl;
//  ofs << originalSize << endl;
  if (originalSize == originalText.size()) ofs << dec << -1 << endl;
  else ofs << dec << originalSize << endl;
  ofs << endl;
  
  ofs << "### original" << endl;
  ofs << "<" << endl;
  ofs << originalText << endl;
  ofs << ">" << endl;
  ofs << endl;
  
  ofs << "### translation" << endl;
  ofs << "<" << endl;
  ofs << translatedText << endl;
  ofs << ">" << endl;
  ofs << "#############################" << endl;
  
  ofs << endl;
}

void TranslationEntry::load(std::istream& ifs) {
  advance(ifs);
  ifs >> sourceFile;
  
  advance(ifs);
  ifs >> hex >> sourceFileOffset;
  
  advance(ifs);
  unsigned int numPointers;
  ifs >> numPointers;

  for (unsigned int i = 0; i < numPointers; i++) {
    advance(ifs);
    unsigned int p;
    ifs >> p;
    pointers.push_back(p);
  }
  
  advance(ifs);
  ifs >> dec >> originalSize;
  
  readStringLiteral(ifs, originalText);
  readStringLiteral(ifs, translatedText);
  
  if ((signed int)originalSize == -1) originalSize = originalText.size();
}
  
void TranslationFile::load(std::istream& ifs) {
  while (ifs.good()) {
    TranslationEntry entry;
    entry.load(ifs);
//    cout << entry.sourceFileOffset << endl;
    filenameEntriesMap[entry.sourceFile].push_back(entry);
    
    advance(ifs);
  }
}

/*int main(int argc, char* argv[]) {
  TranslationEntry e;
//  e.sourceFile = "1";
//  e.sourceFileOffset = 12345;
//  e.originalText = "awieubvtoaiweubtv\noianewt\naowietn";
//  e.translatedText = "amwetbio0\naertno\naertn";
//  e.pointers.push_back(54321);
  
  ifstream ifs("mgl_tttest.txt");
  e.load(ifs);
  ifs.close();
  
  e.save(cout);
  
  return 0;
} */
