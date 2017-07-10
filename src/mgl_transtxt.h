#ifndef MGLTRANSTXT_H
#define MGLTRANSTXT_H


#include <string>
#include <vector>
#include <map>

int fsize(std::istream& ifs);

std::string nextToken(std::istream& ifs);
void escapeString(const std::string& src, std::string& dst);
void skipWhitespace(std::istream& ifs);
void advance(std::istream& ifs);
void readStringLiteral(std::istream& ifs, std::string& str);

struct TranslationEntry {

  std::string sourceFile;
  unsigned int sourceFileOffset;
  
  std::string originalText;
  std::string translatedText;
  
  std::vector<unsigned int> pointers;
  
  void save(std::ostream& ofs);
  void load(std::istream& ifs);
  
};

typedef std::vector<TranslationEntry> TranslationEntryCollection;
typedef std::map<std::string, TranslationEntryCollection>
  FilenameTranslationEntryMap;

struct TranslationFile {
  
  FilenameTranslationEntryMap filenameEntriesMap;
  
  void load(std::istream& ifs);
};

struct FreeSpaceEntry {
  
  unsigned int offset;
  unsigned int size;
  
};

struct FreeSpaceList {
  
};


#endif 
