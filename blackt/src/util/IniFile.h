#ifndef INIFILE_H
#define INIFILE_H


#include <string>
#include <map>
#include <utility>

namespace BlackT {


typedef std::map<std::string, std::string> StringMap;
typedef std::pair<std::string, std::string> StringPair;
typedef StringMap KeyValueMap;
typedef std::map<std::string, StringMap> SectionKeysMap;
typedef std::pair<std::string, StringMap> SectionKeysPair;

class IniFile {
public:
  IniFile();
  IniFile(const std::string& filename);
  
  void readFile(const std::string& filename);
  void writeFile(const std::string& filename);
  
  std::string sourceFileName() const;
  
  bool hasSection(const std::string& section) const;
  bool hasKey(const std::string& section,
              const std::string& key) const;
  std::string valueOfKey(const std::string& section,
                         const std::string& key) const;
  void setValue(const std::string& section,
                const std::string& key,
                const std::string& value);
  void clearSection(const std::string& section);
  void clearKey(const std::string& section,
                const std::string& key);
  
protected:
  std::string sourceFileName_;
  
  SectionKeysMap values_;
  
};


};


#endif
