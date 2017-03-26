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

struct VayMonster {
  
  const static int dataSize = 0x40;
  
  int a; //1b
  int b; //1b
  int c; //1b
  string name;
  int d; // 1b
  char data[dataSize];
  
  int e;
  int hp;
  int attack;
  int defense;
  int exp;
  int gold;
  int bitfield;
  int crit;
  int speed;
  int f;
  int g;
  int h;
  int i;
  int j;
  int k;
  int l;
  
  
  void read(istream& ifs) {
    a = readInt(ifs, 1);
    b = readInt(ifs, 1);
    c = readInt(ifs, 1);
    
    int namelen = readInt(ifs, 1);
    for (int i = 0; i < namelen; i++) {
      name += ifs.get();
    }
    
    d = readInt(ifs, 1);
    
    ifs.read(data, dataSize);
    
    e = ByteConversion::fromBytes(data + (0 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    hp = ByteConversion::fromBytes(data + (1 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    attack = ByteConversion::fromBytes(data + (2 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    defense = ByteConversion::fromBytes(data + (3 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    exp = ByteConversion::fromBytes(data + (4 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    gold = ByteConversion::fromBytes(data + (5 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    bitfield = ByteConversion::fromBytes(data + (6 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    crit = ByteConversion::fromBytes(data + (7 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    speed = ByteConversion::fromBytes(data + (8 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    f = ByteConversion::fromBytes(data + (9 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    g = ByteConversion::fromBytes(data + (10 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    h = ByteConversion::fromBytes(data + (11 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    i = ByteConversion::fromBytes(data + (12 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    j = ByteConversion::fromBytes(data + (13 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    k = ByteConversion::fromBytes(data + (14 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
    l = ByteConversion::fromBytes(data + (15 * 4) + 1,
                                  3,
                                  EndiannessTypes::big,
                                  SignednessTypes::nosign);
  }
  
  void write(ostream& ofs) {
    writeInt(ofs, a, 1);
    writeInt(ofs, b, 1);
    writeInt(ofs, c, 1);
    
    writeInt(ofs, name.size(), 1);
    ofs.write(name.c_str(), name.size());
    
    writeInt(ofs, d, 1);
    
    ofs.write(data, dataSize);
  }
  
  void print(ostream& ofs) {
    cout << '\t' << "a: " << dec << a << endl;
    cout << '\t' << "b: " << b << endl;
    cout << '\t' << "c: " << c << endl;
    cout << '\t' << "name: " << name << endl;
    cout << '\t' << "d: " << d << endl;
    cout << '\t' << "data:";
    for (int i = 0; i < dataSize; i++) {
      cout << " " << hex << (unsigned int)(((unsigned char*)(data))[i]);
    }
    cout << endl;
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

void compareEnemy(VayMonster jp, VayMonster us) {
//  printAsDiff(jp.e, us.e, 0.50);
  printAsDiff(jp.hp, us.hp, 0.50);
  printAsDiff(jp.attack, us.attack, 0.50);
  printAsDiff(jp.defense, us.defense, 0.50);
  printAsDiff(jp.speed, us.speed, 0.50);
  printAsDiff(jp.crit, us.crit, 0.50);
  printAsDiff(jp.exp, us.exp, -0.50, true);
  printAsDiff(jp.gold, us.gold, -0.50, true);
/*  cout << " | style=\"text-align: center;\" | ";
  if (jp.bitfield == us.bitfield) {
    cout << "<span style=\"color: gray;\">" << hex << setw(6) << jp.bitfield
            << "</span>\n";
  }
  else {
    cout << hex << setw(6) << jp.bitfield;
    cout    << " -> ";
    cout << hex << setw(6) << us.bitfield;
    cout    << "\n\n";
//    printChange(jp.bitfield, us.bitfield, 0.50, false);
    cout    << "\n";
  } */
  printAsDiff(jp.f, us.f, 0.50);
//  printAsDiff(jp.g, us.g, 0.50);
//  printAsDiff(jp.h, us.h, 0.50);
  printAsDiff(jp.i, us.i, 0.50);
  printAsDiff(jp.j, us.j, 0.50);
  printAsDiff(jp.k, us.k, 0.50);
  printAsDiff(jp.l, us.l, 0.50);
  
}

bool comparisonNeeded(VayMonster jp, VayMonster us) {
  if ((jp.e == us.e)
      && (jp.hp == us.hp)
      && (jp.attack == us.attack)
      && (jp.defense == us.defense)
      && (jp.exp == us.exp)
      && (jp.gold == us.gold)
      && (jp.bitfield == us.bitfield)
      && (jp.crit == us.crit)
      && (jp.speed == us.speed)
      && (jp.f == us.f)
      && (jp.g == us.g)
      && (jp.h == us.h)
      && (jp.i == us.i)
      && (jp.j == us.j)
      && (jp.k == us.k)
      && (jp.l == us.l)) {
    return false;
  }
  
  return true;
}

void compareEnemies(vector<VayMonster>& enemies_jp,
                    vector<VayMonster>& enemies_us) {
  cout << "{| class=\"wikitable\" style=\"margin: 1em auto 1em auto;\"\n"
       << " ! Name\n"
//       << " ! ? \n"
       << " ! HP \n"
       << " ! Attack \n"
       << " ! Defense \n"
       << " ! Speed \n"
       << " ! Crit % \n"
       << " ! EXP \n"
       << " ! Gold \n"
//       << " ! Bitfield \n"
       << " ! ? \n"
//       << " ! ? \n"
//       << " ! ? \n"
       << " ! ? \n"
       << " ! ? \n"
       << " ! ? \n"
       << " ! ? \n";
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

const static int usStart = 0xE;
const static int usEnd = 0x2B86;
const static int jpStart = 0xE;
const static int jpEnd = 0x2CC8;

int main(int argc, char* argv[]) {
  
  if (argc < 3) {
//    cout << "Usage: vaydecmp <infile> [outfile]" << endl;
    return 0;
  }
  
  string usfile(argv[1]);
  string jpfile(argv[2]);
  
//  string usfile(argv[1]);
  
  ifstream ifs(usfile.c_str(), ios_base::binary);
  
  vector<VayMonster> usMonsters;
  ifs.seekg(usStart);
  while (ifs.tellg() < usEnd) {
    VayMonster monster;
    monster.read(ifs);
    usMonsters.push_back(monster);
    
//    cout << "---Monster---" << endl;
//    monster.print(cout);
//    char c; cin >> c;
  }
  
/*  for (int i = 0; i < usMonsters.size(); i++) {
    cout << "---Monster " << hex << i << "---" << endl;
    usMonsters[i].print(cout);
    cout << endl;
  } */
  
  ifs.close();
  ifs.open(jpfile.c_str(), ios_base::binary);
  vector<VayMonster> jpMonsters;
  ifs.seekg(jpStart);
  while (ifs.tellg() < jpEnd) {
    VayMonster monster;
    monster.read(ifs);
    jpMonsters.push_back(monster);
  }
  
/*  for (int i = 0; i < jpMonsters.size(); i++) {
    cout << "---Monster " << hex << i << "---" << endl;
    jpMonsters[i].print(cout);
    cout << endl;
  } */
  
  for (int i = 0; i < usMonsters.size(); i++) {
/*    usMonsters[i].price = jpMonsters[i].price;
    usMonsters[i].attack = jpMonsters[i].attack;
    usMonsters[i].defense = jpMonsters[i].defense;
    usMonsters[i].speed = jpMonsters[i].speed;
    usMonsters[i].wisdom = jpMonsters[i].wisdom;
    
    usMonsters[i].write(cout); */
    
    // conversion?
/*    usMonsters[i].a = jpMonsters[i].a;
    usMonsters[i].b = jpMonsters[i].b;
    usMonsters[i].c = jpMonsters[i].c;
    usMonsters[i].d = jpMonsters[i].d;
    memcpy(usMonsters[i].data, jpMonsters[i].data, VayMonster::dataSize);
    usMonsters[i].write(cout); */
    
/*    if (jpMonsters[i].price != usMonsters[i].price) {
      cout << "---Monster " << hex << i << "---" << endl;
      jpMonsters[i].print(cout);
      cout << endl;
      usMonsters[i].print(cout);
      cout << endl;
      cout << endl;
    } */
    
/*    if ((jpMonsters[i].a != usMonsters[i].a)
        || (jpMonsters[i].b != usMonsters[i].b)
        || (jpMonsters[i].c != usMonsters[i].c)
        || (jpMonsters[i].d != usMonsters[i].d)) {
      cout << "---Monster " << hex << i << "---" << endl;
      jpMonsters[i].print(cout);
      cout << endl;
      usMonsters[i].print(cout);
      cout << endl;
      cout << endl;
    } */
    
/*    if (memcmp(jpMonsters[i].data, usMonsters[i].data, VayMonster::dataSize)
          != 0) {
      cout << "---Monster " << hex << i << "---" << endl;
      jpMonsters[i].print(cout);
      cout << endl;
      usMonsters[i].print(cout);
      cout << endl;
      cout << endl;
    } */
  }
  
  compareEnemies(jpMonsters, usMonsters);
  
  return 0;
}
