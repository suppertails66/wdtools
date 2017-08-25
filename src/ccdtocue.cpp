#include "util/IniFile.h"
#include "util/TStringConversion.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;
using namespace BlackT;

const static int pregapSizeFrame = 150;
const static int pregapSizeSec = 2;

const static int frameRange = 75;
const static int secRange = 60;

struct MsfNumber {
  int min;
  int sec;
  int frame;
  
  MsfNumber operator+(const MsfNumber& other) {
    int newmin = min;
    int newsec = sec;
    int newframe = frame;
  
    newframe += other.frame;
    if (newframe >= frameRange) {
      newframe -= frameRange;
      ++newsec;
    }
    else if (newframe < 0) {
      newframe += frameRange;
      --newsec;
    }
    
    newsec += other.sec;
    if (newsec >= secRange) {
      newsec -= secRange;
      ++newmin;
    }
    else if (newsec < 0) {
      newsec += secRange;
      --newmin;
    }
    
    newmin += other.min;
    
    return MsfNumber { newmin, newsec, newframe };
  }
  
  MsfNumber operator-(const MsfNumber& other) {
    MsfNumber conv { -(other.min), -(other.sec), -(other.frame) };
    return (*this + conv);
  }
};

int findExtensionPosition(const string& str) {
  // names beginning with a period have no extension
  for (int i = str.size() - 1; i >= 1; i--) {
    if (str[i] == '.') return i;
  }
  
  return -1;
}

string getBaseName(const string& str) {
  int extensionpos = findExtensionPosition(str);
  if (extensionpos < 0) return str;
  
  return str.substr(0, extensionpos);
}

bool fileExists(const string& name) {
  ifstream ifs(name.c_str(), ios_base::binary);
  return (ifs.tellg() >= 0);
}

int main(int argc, char* argv[]) {
  if (argc < 3) return 0;
  
  string infilename(argv[1]);
  string outfilename(argv[2]);
  
  // compute name of img file
  string imgFileName = getBaseName(infilename);
  
  // check for img and bin extensions
  if (fileExists(imgFileName + ".img")) imgFileName += ".img";
  else if (fileExists(imgFileName + ".bin")) imgFileName += ".bin";
  else {
    cerr << "No matching image file for name '" << imgFileName << "'" << endl;
    return 1;
  }
  
  // read the ccd
  IniFile ccd = IniFile(infilename);
  
  // prepare for output
  ofstream ofsFile(outfilename.c_str());
  ostream& ofs = ofsFile;
  
  int numTocEntries
    = TStringConversion::stringToInt(ccd.valueOfKey("Disc", "TocEntries"));
  
  // output header
  ofs << "FILE \"" << imgFileName << "\" BINARY" << endl;

//  MsfNumber test1 { 2, 3, 4 };
//  MsfNumber test2 { 1, 2, 5 };
//  MsfNumber result = test1 - test2;
//  cout << result.min << " " << result.sec << " " << result.frame << endl;

  int cdMode = TStringConversion::stringToInt(
    ccd.valueOfKey("Entry 0", "PSec"));
  
  // entries starting from 3 are tracks
  int tracknum = 1;
  for (int i = 3; i < numTocEntries; i++) {
    string section = "Entry " + TStringConversion::toString(i);
    MsfNumber pos;
    pos.min
      = TStringConversion::stringToInt(ccd.valueOfKey(section, "PMin"));
    pos.sec
      = TStringConversion::stringToInt(ccd.valueOfKey(section, "PSec"));
    pos.frame
      = TStringConversion::stringToInt(ccd.valueOfKey(section, "PFrame"));
    
    // ccd uses a fixed-size pregap -- subtract to get actual position
    MsfNumber pregap { 0, pregapSizeSec, 0 };
    MsfNumber pregapPos = pos - pregap;
    
//    cout << pos.min << " " << pos.sec << " " << pos.frame << endl;

    int adr = TStringConversion::stringToInt(
      ccd.valueOfKey(section, "ADR"));
    int control = TStringConversion::stringToInt(
      ccd.valueOfKey(section, "Control"));
      
    ofs << setfill('0');
    ofs << "  TRACK " << dec << setw(2) << tracknum << " ";
    
    // assume track 1 is (the only) cd-rom track
    if (tracknum == 1) {
      switch (cdMode) {
      case 0:
        ofs << "MODE1/2352" << endl;
        break;
      case 1:
        ofs << "CDI/2336" << endl;
        break;
      case 2:
        ofs << "MODE2/2352" << endl;
        break;
      default:
        cerr << "invalid cdMode" << endl;
        return 1;
        break;
      }
      
      ofs << "    INDEX 01 " << dec
        << setw(2) << pregapPos.min << ":"
        << setw(2) << pregapPos.sec << ":"
        << setw(2) << pregapPos.frame << endl;
    }
    else {
      ofs << "AUDIO" << endl;
      
      pregapPos = (pregapPos - MsfNumber { 0, pregapSizeSec, 0 });
      pos = (pos - MsfNumber { 0, pregapSizeSec, 0 });
      ofs << "    INDEX 00 " << dec
        << setw(2) << pregapPos.min << ":"
        << setw(2) << pregapPos.sec << ":"
        << setw(2) << pregapPos.frame << endl;
      ofs << "    INDEX 01 " << dec
        << setw(2) << pos.min << ":"
        << setw(2) << pos.sec << ":"
        << setw(2) << pos.frame << endl;
    }
    
    ++tracknum;
  }
  
//  cout << ccd.valueOfKey("CloneCD", "Version") << endl;
  
  return 0;
} 
