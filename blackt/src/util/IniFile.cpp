#include "util/IniFile.h"
#include <iostream>
#include <fstream>

namespace BlackT {


IniFile::IniFile() { };
IniFile::IniFile(const std::string& filename)
  : sourceFileName_(filename) {
  readFile(filename);
}

void IniFile::readFile(const std::string& filename) {
  std::ifstream ifs(filename);
  std::string currentSection = "";
  while (ifs.good()) {
    std::string line;
    getline(ifs, line);
    
    if (line.size() <= 1) {
      continue;
    }
    
    // Comment
    if ((line[0] == ';') || (line[0] == '#')) {
      continue;
    }
    
    int start = 0;
    int end = 0;
    
    // Section
    if (line[0] == '[') {
      start = 1;
      end = 1;
      bool failed = false;
      while (line[end] != ']') {
        
        ++end;
      
        // No closing bracket: ignore line
        if (end >= line.size()) {
          failed = true;
          break;
        }
      }
      
      if (failed) {
        continue;
      }
      
      currentSection = line.substr(start, (end - start));
    }
    // Key-value pair
    else {
      start = 0;
      end = 0;
      bool failed = false;
      while (line[end] != '=') {
      
        // No equals sign: ignore line
        if (end >= line.size()) {
          failed = true;
          break;
        }
        
        ++end;
      }
      
      if (failed) {
        continue;
      }
      
      std::string key = line.substr(start, (end - start));
    
      start = end + 1;
      end = start;
      
      if (start >= line.size()) {
        continue;
      }
      
      std::string value = line.substr(start, (line.size() - start));
      
      setValue(currentSection, key, value);
    }
    
  }
}

void IniFile::writeFile(const std::string& filename) {
  std::ofstream ofs(filename);
  for (SectionKeysMap::iterator it = values_.begin();
       it != values_.end();
       ++it) {
     ofs << '[' << it->first << ']' << std::endl;
     
     for (StringMap::iterator jt = it->second.begin();
          jt != it->second.end();
          ++jt) {
       ofs << jt->first << '=' << jt->second << std::endl;
     }
     
     ofs << std::endl;
  }
}

std::string IniFile::sourceFileName() const {
  return sourceFileName_;
}

bool IniFile::hasSection(const std::string& section) const {
  if (values_.find(section) != values_.end()) {
    return true;
  }
  
  return false;
}

bool IniFile::hasKey(const std::string& section,
            const std::string& key) const {
  SectionKeysMap::const_iterator findIt = values_.find(section);
  if (findIt == values_.end()) {
    return false;
  }
  
  StringMap::const_iterator valueIt = findIt->second.find(key);
  
  if (valueIt == findIt->second.end()) {
    return false;
  }
  
  return true;
}

std::string IniFile::valueOfKey(const std::string& section,
                       const std::string& key) const {
//  return values_[section][key];

  SectionKeysMap::const_iterator findIt = values_.find(section);
  StringMap::const_iterator valueIt = findIt->second.find(key);
  return valueIt->second;
}

void IniFile::setValue(const std::string& section,
              const std::string& key,
              const std::string& value) {
  values_[section][key] = value;
}

void IniFile::clearSection(const std::string& section) {
  SectionKeysMap::iterator findIt = values_.find(section);
  
  if (findIt == values_.end()) {
    return;
  }

  values_.erase(values_.find(section));
}

void IniFile::clearKey(const std::string& section,
              const std::string& key) {
  SectionKeysMap::iterator findIt = values_.find(section);
  
  if (findIt == values_.end()) {
    return;
  }
  
  StringMap::iterator valueIt = findIt->second.find(key);
  
  if (valueIt == findIt->second.end()) {
    return;
  }
  
  findIt->second.erase(valueIt);
}


};
