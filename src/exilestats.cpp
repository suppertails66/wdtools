#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"

using namespace std;
using namespace BlackT;

const static int blockSize = 0x800;
const static int usMasterLoadTableOffset = 0xB1B21;
const static int jpMasterLoadTableOffset = 0xB1826;
const static int numMasterLoadTableEntries = 0x26;
const static int usMasterLoadOffsetShift = 0x40;
const static int loadedSegmentBufferSize = 0x2000;
const static int enemyStatsTablePointerOffset = 0x001C;
const static int enemyStatsTablePointerSize = 2;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

struct MasterLoadTableEntry {
  
  int blockOffset;
  int loadAddress;
  
  bool read(istream& ifs, bool us) {
    blockOffset = TSerialize::readInt(ifs, 3, EndiannessTypes::little,
                                              SignednessTypes::nosign);
    loadAddress = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                                              SignednessTypes::nosign);
                                              
    if ((blockOffset == 0) && (loadAddress == 0)) {
      return false;
    }
    
    if (us) blockOffset += usMasterLoadOffsetShift;
    blockOffset *= blockSize;
    loadAddress <<= 8;
    return true;
  }
  
};

struct MasterLoadTable {
  
  vector<MasterLoadTableEntry> entries;
  
  void read(istream& ifs, bool us) {
    entries.clear();
    for (int i = 0; i < numMasterLoadTableEntries; i++) {
      MasterLoadTableEntry entry;
      if (entry.read(ifs, us)) {
        entries.push_back(entry);
//        cout << hex << ifs.tellg() << endl;
      }
    }
//    cout << endl;
  }
  
};

struct EnemyStatsTableEntry {

  const static int size = 8;
  
  int hp;
  int attack;
  int defense;
  int a;
  int b;
  int c;
  int d;
  int e;
  
  void read(istream& ifs) {
    hp = TSerialize::readInt(
      ifs, 1, EndiannessTypes::little, SignednessTypes::nosign);
    attack = TSerialize::readInt(
      ifs, 1, EndiannessTypes::little, SignednessTypes::nosign);
    defense = TSerialize::readInt(
      ifs, 1, EndiannessTypes::little, SignednessTypes::nosign);
    a = TSerialize::readInt(
      ifs, 1, EndiannessTypes::little, SignednessTypes::nosign);
    b = TSerialize::readInt(
      ifs, 1, EndiannessTypes::little, SignednessTypes::nosign);
    c = TSerialize::readInt(
      ifs, 1, EndiannessTypes::little, SignednessTypes::nosign);
    d = TSerialize::readInt(
      ifs, 1, EndiannessTypes::little, SignednessTypes::nosign);
    e = TSerialize::readInt(
      ifs, 1, EndiannessTypes::little, SignednessTypes::nosign);
  }
  
  void write(ostream& ofs) {
    TSerialize::writeInt(
      ofs, hp, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      ofs, attack, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      ofs, defense, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      ofs, a, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      ofs, b, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      ofs, c, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      ofs, d, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      ofs, e, 1, EndiannessTypes::little, SignednessTypes::nosign);
  }
  
  void write(char* dst) {
    TSerialize::writeInt(
      dst++, hp, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      dst++, attack, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      dst++, defense, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      dst++, a, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      dst++, b, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      dst++, c, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      dst++, d, 1, EndiannessTypes::little, SignednessTypes::nosign);
    TSerialize::writeInt(
      dst++, e, 1, EndiannessTypes::little, SignednessTypes::nosign);
  }
  
  void print(ostream& ofs) {
    ofs << "\tHP:  " << hp << endl
        << "\tAtk: " << attack << endl
        << "\tDef: " << defense << endl
        << "\tEXP: " << a << endl
        << "\tB:   " << b << endl
        << "\tC:   " << c << endl
        << "\tD:   " << d << endl
        << "\tE:   " << e << endl;
  }
  
};

struct EnemyStatsTable {
  
  vector<EnemyStatsTableEntry> entries;
  int sourceAddress;
  
  void readFromMasterLoad(istream& ifs, MasterLoadTableEntry entry) {
    entries.clear();
  
//    ifs.seekg(entry.blockOffset);
//    char buffer[loadedSegmentBufferSize];
//    ifs.read(buffer, loadedSegmentBufferSize);
    
/*    int statsTableOffset = ByteConversion::fromBytes(
      buffer + enemyStatsTablePointerOffset,
      enemyStatsTablePointerSize,
      EndiannessTypes::little,
      SignednessTypes::nosign);
    int nextOffset = ByteConversion::fromBytes(
      buffer + enemyStatsTablePointerOffset + 4,
      enemyStatsTablePointerSize,
      EndiannessTypes::little,
      SignednessTypes::nosign); */
    ifs.seekg(entry.blockOffset + enemyStatsTablePointerOffset);
    int statsTableOffset = TSerialize::readInt(
      ifs,
      enemyStatsTablePointerSize,
      EndiannessTypes::little,
      SignednessTypes::nosign);
    ifs.seekg(entry.blockOffset + enemyStatsTablePointerOffset + 4);
    int nextOffset = TSerialize::readInt(
      ifs,
      enemyStatsTablePointerSize,
      EndiannessTypes::little,
      SignednessTypes::nosign);
    
    int tableSize = (nextOffset - statsTableOffset);
    int numEntries = tableSize / EnemyStatsTableEntry::size;
    
    statsTableOffset -= entry.loadAddress;
    sourceAddress = entry.blockOffset + statsTableOffset;
    
    ifs.seekg(sourceAddress);
    
    if (statsTableOffset < 0) {
      cerr << "underrun" << endl;
      return;
    }
    else if ((statsTableOffset + tableSize) >= loadedSegmentBufferSize) {
      cerr << "overrun" << endl;
      return;
    }
    
    for (int i = 0; i < numEntries; i++) {
      EnemyStatsTableEntry entry;
      entry.read(ifs);
      entries.push_back(entry);
    }
    
  }
  
  void write(ostream& ofs) {
    ofs.seekp(sourceAddress);
    for (unsigned int i = 0; i < entries.size(); i++) {
      entries[i].write(ofs);
    }
  }
  
  void write(char* dst) {
    for (unsigned int i = 0; i < entries.size(); i++) {
      entries[i].write(dst);
      dst += EnemyStatsTableEntry::size;
    }
  }
  
};

void printStat(int amount, bool hpTriggered = false) {
  cout << "<span style=\"font-weight: bold; ";
//  if (hpTriggered) {
//    cout << "color: red; ";
//  }
  cout << "\">";
  cout    << dec << setw(1) << amount;
  cout << "</span>";
}

void printChange(int jp, int us, double triggerRatio, bool reverse = false) {
  cout << "<span style=\"font-weight: bold; ";
  bool usGreater = (us > jp);
  double ratio = ((double)(us) / (double)(jp)) - 1.00;
  int amount = us - jp;
  
  if (reverse) {
    if (!usGreater) {
      if ((ratio < triggerRatio) && (abs(amount) > 5)) {
        cout << "color: magenta; ";
      }
      else {
        cout << "color: red; ";
      }
    }
    else {
      cout << "color: green; ";
    }
  }
  else {
    if (usGreater) {
      if ((ratio >= triggerRatio) && (abs(amount) > 5)) {
        cout << "color: magenta; ";
      }
      else {
        cout << "color: red; ";
      }
    }
    else {
      cout << "color: green; ";
    }
  }
  cout << "\">";
  
  if (usGreater) {
    cout << "+";
  }
//  else {
//    cout << "-";
//  }

//  if (reverse && (ratio < 0)) {
//    ratio = -ratio;
//  }
  
  cout << dec << setw(1) << amount;
  cout << ", ";
//  if (reverse && (us == 0)) {
//    cout << "-%";
//  }
//  else
  if (jp == 0) {
    cout << "-%";
  }
  else {
    cout << fixed << setprecision(0) << (ratio * 100) << "%";
  }
  cout << "</span>";
}

void printAsDiff(int jp, int us, double ratio, bool reverse = false) {
  cout << " | style=\"text-align: center;\" | ";
  if (jp == us) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << jp
            << "</span>\n";
  }
  else {
    printStat(jp);
    cout    << " -> ";
    printStat(us);
    cout    << "\n\n";
    printChange(jp, us, ratio, reverse);
    cout    << "\n";
  }
}

void compareEnemy(EnemyStatsTableEntry jp, EnemyStatsTableEntry us) {
  printAsDiff(jp.hp, us.hp, 0.50);
  printAsDiff(jp.attack, us.attack, 0.50);
  printAsDiff(jp.defense, us.defense, 0.50);
//  printAsDiff(jp.a, us.a, 0.50);
  printAsDiff(jp.b, us.b, -0.50, true);
//  printAsDiff(jp.c, us.c, 0.50);
//  printAsDiff(jp.d, us.d, 0.50);
//  printAsDiff(jp.e, us.e, 0.50);
  
}

bool comparisonNeeded(EnemyStatsTableEntry jp, EnemyStatsTableEntry us) {
  if ((jp.hp == us.hp)
      && (jp.attack == us.attack)
      && (jp.defense == us.defense)
      && (jp.a == us.a)
      && (jp.b == us.b)
      && (jp.c == us.c)
      && (jp.d == us.d)
      && (jp.e == us.e)) {
    return false;
  }
  
  return true;
}

void compareEnemies(vector<EnemyStatsTableEntry>& enemies_jp,
                    vector<EnemyStatsTableEntry>& enemies_us,
                    int stage) {
  bool exists = false;
  for (int i = 0; i < enemies_jp.size(); i++) {
    if (comparisonNeeded(enemies_jp[i], enemies_us[i])) {
      exists = true;
      break;
    }
  }
  if (!exists) {
    return;
  }
  
  map<int, string> stageNames;
  stageNames[0] = "Antlion's Nest";
  stageNames[1] = "El-In";
  stageNames[2] = "Mosnee Pass Cathedral (Original)";
  stageNames[3] = "Albee Passage";
  stageNames[4] = "Mosnee Pass Cathedral (\"Broken\")";
  stageNames[5] = "Kishuk Woods";
  stageNames[6] = "Baiyon";
  stageNames[7] = "Mount Koya";
  stageNames[8] = "Kongobu Temple";
  stageNames[9] = "Homis Shrine";
  stageNames[10] = "Shrine of Backos";
  stageNames[11] = "?";
  stageNames[12] = "Croton Academy";
  stageNames[13] = "Garden of Eden";
  stageNames[14] = "Shrine of Backos (Duplicate)";
  
  cout << "{| class=\"wikitable\" style=\"margin: 1em auto 1em auto;\"\n"
       << " |+ Stage " << (stage + 1) << " -- "
         << stageNames[stage] << "\n"
       << " ! ID\n"
       << " ! width=\"100px\" | HP \n"
       << " ! width=\"100px\" | Attack \n"
       << " ! width=\"100px\" | Defense \n"
//       << " ! width=\"100px\" | EXP \n"
       << " ! width=\"100px\" | Gold \n";
//       << " ! ? \n"
//       << " ! ? \n"
//       << " ! ? \n";
  
  for (int i = 0; i < enemies_jp.size(); i++) {
    if (!comparisonNeeded(enemies_jp[i], enemies_us[i])) {
      continue;
    }
    
    cout << " |-\n";
//    cout << " | style=\"text-align: center; font-weight: bold;\""
//            << " | " << enemies_us[i].name << "\n";
    cout << " | style=\"text-align: center; font-weight: bold;\""
            << " | {{hex|" << setw(2) << setfill('0') << hex << i << "}} \n";
    compareEnemy(enemies_jp[i], enemies_us[i]);
  }
  cout << " |}\n\n";
}

void readStatsTables(ifstream& ifs, vector<EnemyStatsTable>& dst,
                     MasterLoadTable& table) {
  for (unsigned int i = 0; i < table.entries.size(); i++) {
    EnemyStatsTable statsTable;
    statsTable.readFromMasterLoad(ifs, table.entries[i]);
    dst.push_back(statsTable);
  }
}

int main(int argc, char* argv[]) {
  
  if (argc < 3) {
    cout << "Usage: exilestats <usfile> <jpfile> [output]" << endl;
    return 0;
  }
  
  string usfile(argv[1]);
  string jpfile(argv[2]);
  
  ifstream ifs(usfile.c_str(), ios_base::binary);
  ifs.seekg(usMasterLoadTableOffset);
  MasterLoadTable usLoadTable;
  usLoadTable.read(ifs, true);
  vector<EnemyStatsTable> usStatsTables;
  readStatsTables(ifs, usStatsTables, usLoadTable);
  
  ifs.close();
  ifs.open(jpfile.c_str(), ios_base::binary);
  ifs.seekg(jpMasterLoadTableOffset);
  MasterLoadTable jpLoadTable;
  jpLoadTable.read(ifs, true);
  vector<EnemyStatsTable> jpStatsTables;
  readStatsTables(ifs, jpStatsTables, jpLoadTable);
  ifs.close();
  
/*  for (unsigned int i = 0; i < usStatsTables.size(); i++) {
    cout << i << ": " << endl;
    cout << "\tUS: " << hex << usStatsTables[i].sourceAddress << endl;
    cout << "\tJP: " << hex << jpStatsTables[i].sourceAddress << endl;
    cout << endl;
  } */
    
  for (unsigned int i = 0; i < usStatsTables.size(); i++) {
    EnemyStatsTable& usTable = usStatsTables[i];
    EnemyStatsTable& jpTable = jpStatsTables[i];
//    cout << " |+ Stage " << i << endl;
    compareEnemies(jpTable.entries, usTable.entries, i);
  }

  if (argc >= 4) {
    
    ifs.open(usfile.c_str(), ios_base::binary);
    int outfileSize = fsize(ifs);
    char* outbuffer = new char[outfileSize];
    ifs.read(outbuffer, outfileSize);
  
    for (unsigned int i = 0; i < usStatsTables.size(); i++) {
      
  //    cout << "Table " << i << endl;
      
      EnemyStatsTable& usTable = usStatsTables[i];
      EnemyStatsTable& jpTable = jpStatsTables[i];
      
  //    cout << usTable.entries.size() << endl;
  //    cout << jpTable.entries.size() << endl;
  //    cout << endl;
      
      for (unsigned int j = 0; j < usTable.entries.size(); j++) {
        EnemyStatsTableEntry& usEntry = usTable.entries[j];
        EnemyStatsTableEntry& jpEntry = jpTable.entries[j];
      
        // correct
      usEntry.hp = jpEntry.hp;
        usEntry.attack = jpEntry.attack;
        usEntry.defense = jpEntry.defense;
        usEntry.a = jpEntry.a;
        usEntry.b = jpEntry.b;
        usEntry.c = jpEntry.c;
        usEntry.d = jpEntry.d;
        usEntry.e = jpEntry.e;
        
        // print
  /*      if ((usEntry.hp != jpEntry.hp)
            || (usEntry.attack != jpEntry.attack)
            || (usEntry.defense != jpEntry.defense)
            || (usEntry.a != jpEntry.a)
            || (usEntry.b != jpEntry.b)
            || (usEntry.c != jpEntry.c)
            || (usEntry.d != jpEntry.d)
            || (usEntry.e != jpEntry.e)) {
          cout << "Entry " << j << " mismatch: " << endl;
          cout << "JP: " << endl;
          jpEntry.print(cout);
          cout << "US: " << endl;
          usEntry.print(cout);
          cout << endl;
        } */
      }
      usTable.write(outbuffer + usTable.sourceAddress);
    }
   
    ofstream ofs(argv[3], ios_base::binary | ios_base::trunc);
    ofs.write(outbuffer, outfileSize);
    delete outbuffer;
  }
  
  return 0;
}
