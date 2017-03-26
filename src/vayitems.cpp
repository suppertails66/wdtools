#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "util/ByteConversion.h"

using namespace std;
using namespace BlackT;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

/*int decompressVay(char* src, char* dst) {
  
  int srcpos = 0;
  int dstpos = 0;
  
  int dataSize = ByteConversion::fromBytes(src + srcpos,
                           4,
                           EndiannessTypes::big,
                           SignednessTypes::nosign);
  src += 4;
  
  while (srcpos < dataSize) {
    char next = src[srcpos++];
    
    for (int mask = 0x01; mask <= 0x80; mask <<= 1) {
      if (next & mask) {
        // literal
        dst[dstpos++] = src[srcpos++];
      }
      else {
        // lookback
        int offset = (unsigned char)(src[srcpos++]);
        unsigned char second = (unsigned char)(src[srcpos++]);
        offset |= ((int)(second & 0xF0)) << 4;
        int length = (second & 0x0F) + 3;
        
        // lookback offset is 0x12 less than actual
        offset += 0x12;
        // wrap around
        offset = (offset & 0xFFF);
        
        // block starts at previous 0x1000 byte boundary, or 0x0000
        // in first block
        int target;
        int blockBase;
      
        blockBase = ((dstpos / 0x1000) * 0x1000);
        int localPos = dstpos % 0x1000;
        
        if ((offset >= localPos)) {
          blockBase -= 0x1000;
        }
        
        target = blockBase + offset;
        
        // can't use memcpy -- decompressor relies on definite byte-by-byte
        // copy behavior when source and destination overlap
        for (int i = 0; i < length; i++) {
          dst[dstpos++] = dst[target + i];
        }
      }
    }
  }
  
  return dstpos;
} */

int readInt(istream& ifs, int size) {
  char buffer[16];
  
  ifs.read(buffer, size);
  return ByteConversion::fromBytes(buffer, size,
                   EndiannessTypes::big,
                   SignednessTypes::nosign);
}

void writeInt(ostream& ofs, int value, int size) {
  char buffer[16];
  
  ByteConversion::toBytes(value, buffer, size,
                          EndiannessTypes::big,
                          SignednessTypes::nosign);
  ofs.write(buffer, size);
}

struct VayItem {
  
  int unk1; // 2b
  string name;
  int unk2; // 1b
  int unk3; // 2b
  string description;
  int unk4; // 2b
  int unkx; // 2b
  int unk5; // 1b
  int unk6; // 1b
  int unk7; // 1b
  int unk8; // 1b
  int unk9; // 1b
  int unk10; // 1b
  int price; // 3b
  int attack; // 1b
  int defense; // 1b
  int speed; // 1b
  int wisdom; // 1b
  int unk14;  // 3b
  
  void read(istream& ifs) {
    unk1 = readInt(ifs, 2);
    
    int namelen = readInt(ifs, 1);
    for (int i = 0; i < namelen; i++) {
      name += ifs.get();
    }
    
    unk2 = readInt(ifs, 1);
    unk3 = readInt(ifs, 2);
    
    int desclen = readInt(ifs, 1);
    for (int i = 0; i < desclen; i++) {
      description += ifs.get();
    }
    
    unk4 = readInt(ifs, 2);
    unkx = readInt(ifs, 1);
    unk5 = readInt(ifs, 1);
    unk6 = readInt(ifs, 1);
    unk7 = readInt(ifs, 1);
    unk8 = readInt(ifs, 1);
    unk9 = readInt(ifs, 1);
    unk10 = readInt(ifs, 1);
    price = readInt(ifs, 3);
    attack = readInt(ifs, 1);
    defense = readInt(ifs, 1);
    speed = readInt(ifs, 1);
    wisdom = readInt(ifs, 1);
    unk14 = readInt(ifs, 3);
  }
  
  void write(ostream& ofs) {
    writeInt(ofs, unk1, 2);
    
    writeInt(ofs, name.size(), 1);
    ofs.write(name.c_str(), name.size());
    
    writeInt(ofs, unk2, 1);
    writeInt(ofs, unk3, 2);
    
    writeInt(ofs, description.size(), 1);
    ofs.write(description.c_str(), description.size());
    
    writeInt(ofs, unk4, 2);
    writeInt(ofs, unkx, 1);
    writeInt(ofs, unk5, 1);
    writeInt(ofs, unk6, 1);
    writeInt(ofs, unk7, 1);
    writeInt(ofs, unk8, 1);
    writeInt(ofs, unk9, 1);
    writeInt(ofs, unk10, 1);
    writeInt(ofs, price, 3);
    writeInt(ofs, attack, 1);
    writeInt(ofs, defense, 1);
    writeInt(ofs, speed, 1);
    writeInt(ofs, wisdom, 1);
    writeInt(ofs, unk14, 3);
  }
  
  void print(ostream& ofs) {
    cout << '\t' << "unk1: " << dec << unk1 << endl;
    cout << '\t' << "name: " << name << endl;
    cout << '\t' << "unk2: " << unk2 << endl;
    cout << '\t' << "unk3: " << unk3 << endl;
    cout << '\t' << "description: " << description << endl;
    cout << '\t' << "unk4: " << unk4 << endl;
    cout << '\t' << "unkx: " << unkx << endl;
    cout << '\t' << "unk5: " << unk5 << endl;
    cout << '\t' << "unk6: " << unk6 << endl;
    cout << '\t' << "unk7: " << unk7 << endl;
    cout << '\t' << "unk8: " << unk8 << endl;
    cout << '\t' << "unk9: " << unk9 << endl;
    cout << '\t' << "unk10: " << unk10 << endl;
    cout << '\t' << "price: " << price << endl;
    cout << '\t' << "attack: " << attack << endl;
    cout << '\t' << "defense: " << defense << endl;
    cout << '\t' << "speed: " << speed << endl;
    cout << '\t' << "wisdom: " << wisdom << endl;
    cout << '\t' << "unk14: " << unk14 << endl;
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

void compareEnemy(VayItem jp, VayItem us) {
  cout << " | style=\"text-align: center;\" | ";
  if (jp.attack == us.attack) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << jp.attack
            << "</span>\n";
  }
  else {
    printStat(jp.attack);
    cout    << " -> ";
    printStat(us.attack);
    cout    << "\n\n";
    printChange(jp.attack, us.attack, -0.50, true);
    cout    << "\n";
  }
  
  cout << " | style=\"text-align: center;\" | ";
  if (jp.defense == us.defense) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << jp.defense
            << "</span>\n";
  }
  else {
    printStat(jp.defense);
    cout    << " -> ";
    printStat(us.defense);
    cout    << "\n\n";
    printChange(jp.defense, us.defense, -0.50, true);
    cout    << "\n";
  }
  
  cout << " | style=\"text-align: center;\" | ";
  if (jp.speed == us.speed) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << jp.speed
            << "</span>\n";
  }
  else {
    printStat(jp.speed);
    cout    << " -> ";
    printStat(us.speed);
    cout    << "\n\n";
    printChange(jp.speed, us.speed, -0.50, true);
    cout    << "\n";
  }
  
  cout << " | style=\"text-align: center;\" | ";
  if (jp.wisdom == us.wisdom) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << jp.wisdom
            << "</span>\n";
  }
  else {
    printStat(jp.wisdom);
    cout    << " -> ";
    printStat(us.wisdom);
    cout    << "\n\n";
    printChange(jp.wisdom, us.wisdom, -0.50, true);
    cout    << "\n";
  }
  
  cout << " | style=\"text-align: center;\" | ";
  if (jp.price == us.price) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << jp.price
            << "</span>\n";
  }
  else {
    printStat(jp.price);
    cout    << " -> ";
    printStat(us.price);
    cout    << "\n\n";
    printChange(jp.price, us.price, 0.50, false);
    cout    << "\n";
  }
}

bool comparisonNeeded(VayItem jp, VayItem us) {
  if ((jp.price == us.price)
      && (jp.attack == us.attack)
      && (jp.defense == us.defense)
      && (jp.speed == us.speed)
      && (jp.wisdom == us.wisdom)) {
    return false;
  }
  
  return true;
}

void compareEnemies(vector<VayItem>& enemies_jp,
                    vector<VayItem>& enemies_us) {
  cout << "{| class=\"wikitable\" style=\"margin: 1em auto 1em auto;\"\n"
       << " ! Name\n"
       << " ! Attack \n"
       << " ! Defense \n"
       << " ! Speed \n"
       << " ! Wisdom \n"
       << " ! Price\n";
//       << " |-\n";
  for (int i = 0; i < enemies_jp.size(); i++) {
//    cout << "Enemy " << i << ": " << endl;
//    if (i >= 102) {
//      break;
//    }
    
    if (!comparisonNeeded(enemies_jp[i], enemies_us[i])) {
      continue;
    }
    
    cout << " |-\n";
//    cout << " | style=\"text-align: center; font-weight: bold;\""
//            << " | {{hex|" << hex << uppercase << setw(2) << setfill('0') << i << "}}\n";
    cout << " | style=\"text-align: center; font-weight: bold;\""
            << " | " << enemies_us[i].name << "\n";
    compareEnemy(enemies_jp[i], enemies_us[i]);
  }
  cout << " |}";
}

const static int usStart = 0x2D0D;
const static int usEnd = 0x511D;
const static int jpStart = 0x2E61;
const static int jpEnd = 0x5BA7;

int main(int argc, char* argv[]) {
  
  if (argc < 3) {
//    cout << "Usage: vaydecmp <infile> [outfile]" << endl;
    return 0;
  }
  
  string usfile(argv[1]);
  string jpfile(argv[2]);
  
//  string usfile(argv[1]);
  
  ifstream ifs(usfile.c_str(), ios_base::binary);
  
  vector<VayItem> usItems;
  ifs.seekg(usStart);
  while (ifs.tellg() < usEnd) {
    VayItem item;
    item.read(ifs);
    usItems.push_back(item);
    
//    cout << "---Item---" << endl;
//    item.print(cout);
//    char c; cin >> c;
  }
  
/*  for (int i = 0; i < usItems.size(); i++) {
    cout << "---Item " << hex << i << "---" << endl;
    usItems[i].print(cout);
    cout << endl;
  } */
  
  ifs.close();
  ifs.open(jpfile.c_str(), ios_base::binary);
  vector<VayItem> jpItems;
  ifs.seekg(jpStart);
  while (ifs.tellg() < jpEnd) {
    VayItem item;
    item.read(ifs);
    jpItems.push_back(item);
  }
  
  compareEnemies(jpItems, usItems);
  
  for (int i = 0; i < usItems.size(); i++) {
/*    usItems[i].price = jpItems[i].price;
    usItems[i].attack = jpItems[i].attack;
    usItems[i].defense = jpItems[i].defense;
    usItems[i].speed = jpItems[i].speed;
    usItems[i].wisdom = jpItems[i].wisdom;
    
    usItems[i].write(cout); */
    
/*    if (jpItems[i].price != usItems[i].price) {
      cout << "---Item " << hex << i << "---" << endl;
      jpItems[i].print(cout);
      cout << endl;
      usItems[i].print(cout);
      cout << endl;
      cout << endl;
    } */
    
/*    cout << "---Item " << hex << i << "---" << endl;
    jpItems[i].print(cout);
    cout << endl;
    usItems[i].print(cout);
    cout << endl;
    cout << endl; */
    
/*    if ((jpItems[i].attack != usItems[i].attack)
        || (jpItems[i].defense != usItems[i].defense)
        || (jpItems[i].speed != usItems[i].speed)
        || (jpItems[i].wisdom != usItems[i].wisdom)) {
      cout << "---Item " << hex << i << "---" << endl;
      jpItems[i].print(cout);
      cout << endl;
      usItems[i].print(cout);
      cout << endl;
      cout << endl;
    } */
  }
  
  return 0;
}
