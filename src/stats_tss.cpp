#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "util/TByte.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TSerialize.h"
#include "scriptrip_tss_utils.h"

using namespace std;
using namespace BlackT;

struct EnemyStats {

  const static int size = 0x16;
  
  int address;
  
  int initialHP;
  int hp;
  int initialMP;
  int mp;
  int attack;
  int defense;
  int agility;
  int magicDefense;
  int numberOfMoves;
  int numberOfAttacks;
  int away;
  int a;
  int b;
  int c;
  int d;
  int e;
  int f;
  int g;
  
  static bool isEnemy(char* src, int offset, int limit) {
//    if (limit - offset < 0x16) {
//      return false;
//    }
  
    int first = ByteConversion::fromBytes(src + offset + 0,
                                          2,
                                          EndiannessTypes::big,
                                          SignednessTypes::nosign);
    int second = ByteConversion::fromBytes(src + offset + 2,
                                          2,
                                          EndiannessTypes::big,
                                          SignednessTypes::nosign);
    int third = ByteConversion::fromBytes(src + offset + 4,
                                          2,
                                          EndiannessTypes::big,
                                          SignednessTypes::nosign);
    int fourth = ByteConversion::fromBytes(src + offset + 6,
                                          2,
                                          EndiannessTypes::big,
                                          SignednessTypes::nosign);
    
    // this is used as filler
    if ((first == 0xFEFE)
        || (first == 0x0000)
        || (second == 0x0000)) {
      return false;
    }
    
    if ((first == second) && (third == fourth)) {
      return true;
    }
    
    return false;
  }
  
  int load(const char* src, int offset) {
    address = offset;
    initialHP = TSerialize::readInt(src, offset,
                                    2,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    hp = TSerialize::readInt(src, offset,
                                    2,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    initialMP = TSerialize::readInt(src, offset,
                                    2,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    mp = TSerialize::readInt(src, offset,
                                    2,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    attack = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    defense = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    agility = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    magicDefense = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    numberOfMoves = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    numberOfAttacks = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    away = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    a = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    b = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    c = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    d = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    e = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    f = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    g = TSerialize::readInt(src, offset,
                                    1,
                                    EndiannessTypes::big,
                                    SignednessTypes::nosign);
    return size;
  }
  
  int save(char* dst, int offset) {
//    ByteConversion::toBytes(initialHP, dst + offset,
//                             2,
//                         EndiannessTypes::big,
//                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, initialHP, 2,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, hp, 2,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, initialMP, 2,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, mp, 2,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, attack, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, defense, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, agility, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, magicDefense, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, numberOfMoves, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, numberOfAttacks, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, away, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, a, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, b, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, c, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, d, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, e, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, f, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    TSerialize::writeIntS(dst, offset, g, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    return size;
  }
  
/*  EnemyStats& operator=(const EnemyStats& c) {
    initialHP = c.initialHP;
    hp = c.hp;
    initialMP = c.initialMP;
    mp = c.mp;
    
    return *this;
  } */
  
};

int findEnemies(char* src, int srcSize) {
  int startOffset = ByteConversion::fromBytes(src + block1PointerOffset,
                                              4,
                                              EndiannessTypes::big,
                                              SignednessTypes::nosign);
  int endOffset = ByteConversion::fromBytes(src + block2PointerOffset,
                                              4,
                                              EndiannessTypes::big,
                                              SignednessTypes::nosign);
  
  // sometimes get false hits when searching from chunk start
//  for (int i = startOffset; i < (endOffset - EnemyStats::size); i++) {
  for (int i = (endOffset - EnemyStats::size - 1); i >= startOffset; i--) {
    if (EnemyStats::isEnemy(src, i, endOffset)) return i;
  }
  
  return -1;
}

void readEnemies(char* src, int offset, int limit, vector<EnemyStats>& dst) {
  // Note that enemies are placed into dst in reverse order
  do {
    EnemyStats enemy;
    offset -= enemy.load(src, offset);
//    cout << offset << endl;
    dst.push_back(enemy);
  } while (EnemyStats::isEnemy(src, offset, limit));
}

void printIfChanged(string name, int us, int jp) {
  if (us != jp) {
    cout << name << ": " << dec << jp << " -> " << dec << us << endl;
  }
}

void compareEnemy(const EnemyStats& us, const EnemyStats& jp) {
  printIfChanged("Initial HP", us.initialHP, jp.initialHP);
  printIfChanged("HP", us.hp, jp.hp);
  printIfChanged("Initial MP", us.initialMP, jp.initialMP);
  printIfChanged("MP", us.mp, jp.mp);
  printIfChanged("Attack", us.attack, jp.attack);
  printIfChanged("Defense", us.defense, jp.defense);
  printIfChanged("Agility", us.agility, jp.agility);
  printIfChanged("Magic Defense", us.magicDefense, jp.magicDefense);
  printIfChanged("# of Moves", us.numberOfMoves, jp.numberOfMoves);
  printIfChanged("# of Attacks", us.numberOfAttacks, jp.numberOfAttacks);
  printIfChanged("Away", us.away, jp.away);
  printIfChanged("A", us.a, jp.a);
  printIfChanged("B", us.b, jp.b);
  printIfChanged("C", us.c, jp.c);
  printIfChanged("D", us.d, jp.d);
  printIfChanged("E", us.e, jp.e);
  printIfChanged("F", us.f, jp.f);
  printIfChanged("G", us.g, jp.g);
}

int main(int argc, char* argv[]) {
  
  if (argc < 4) {
    return 0;
  }
  
  string fileNameUS(argv[1]);
  string fileNameJP(argv[2]);
  string outfile(argv[3]);
//  string substitutionFileName(argv[4]);
  
  ifstream ifsUS(fileNameUS.c_str());
  ifstream ifsJP(fileNameJP.c_str());
//  ifstream ifsSubs(substitutionFileName.c_str());
  
  int sizeUS = fsize(ifsUS);
  int sizeJP = fsize(ifsJP);
  
  char* fileUS = new char[sizeUS];
  char* fileJP = new char[sizeJP];
  
  ifsUS.read(fileUS, sizeUS);
  ifsJP.read(fileJP, sizeJP);
  
  int enemiesOffsetUS = findEnemies(fileUS, sizeUS);
  int enemiesOffsetJP = findEnemies(fileJP, sizeJP);
  
//  cout << endl;
  
  if (enemiesOffsetUS == -1) {
//    cout << "Couldn't find US enemies" << endl;
  }
  
  if (enemiesOffsetJP == -1) {
//    cout << "Couldn't find JP enemies" << endl;
  }
  
  if ((enemiesOffsetUS == -1) || (enemiesOffsetJP == -1)) {
    ofstream ofs(outfile.c_str(), ios_base::binary);
    ofs.write(fileUS, sizeUS);
    return 0;
  }
  
//  cout << "US: " << hex << enemiesOffsetUS << endl;
//  cout << "JP: " << hex << enemiesOffsetJP << endl;
  
  vector<EnemyStats> enemiesUS;
  vector<EnemyStats> enemiesJP;
  
  readEnemies(fileUS, enemiesOffsetUS, sizeUS, enemiesUS);
  readEnemies(fileJP, enemiesOffsetJP, sizeJP, enemiesJP);
  
/*  ScriptMessageFile enemyNamesUS;
//  ScriptMessageFile enemyNamesJP;

  int startOffsetUS = ByteConversion::fromBytes(fileUS + block1PointerOffset,
                                              4,
                                              EndiannessTypes::big,
                                              SignednessTypes::nosign);
  int endOffsetUS = ByteConversion::fromBytes(fileUS + block2PointerOffset,
                                              4,
                                              EndiannessTypes::big,
                                              SignednessTypes::nosign);
//  enemyNamesUS.generate((TByte*)fileUS, endOffsetUS, enemiesOffsetUS);
//  enemyNamesUS.generate((TByte*)fileUS, endOffsetUS, startOffsetUS,
//                        1500, 160, 80);

  enemyNamesUS.generate((TByte*)fileUS, endOffsetUS, startOffsetUS,
                        1000);
  
//  for (int i = 0; i < enemyNamesUS.scriptMessages.size(); i++) {
//    cout << i << ": " << enemyNamesUS.scriptMessages[i].data << endl;
//  }
  
  SubstitutionFile subs;
  subs.load(ifsSubs);
  for (int i = 0; i < enemyNamesUS.scriptMessages.size(); i++) {
    subs.doSubstitutions(enemyNamesUS.scriptMessages[i].data);
  }
  enemyNamesUS.print(fileNameUS); */
  
  if (enemiesUS.size() != enemiesJP.size()) {
    cout << "Enemy mismatch: US " << enemiesUS.size()
         << ", JP " << enemiesJP.size() << endl;
    return 0;
  }
  
//  cout << "Enemy count: " << enemiesUS.size() << endl;
  for (unsigned int i = 0; i < enemiesUS.size(); i++) {
//    cout << enemiesUS[i].address << " " << enemiesJP[i].address << endl;

//    cout << "Enemy " << i << endl;
//    compareEnemy(enemiesUS[i], enemiesJP[i]);

    int offset = enemiesUS[i].address;
//    enemiesUS[i] = enemiesJP[i];
    enemiesJP[i].save(fileUS, offset);
//    cerr << enemiesUS[i].initialHP << " " << enemiesJP[i].initialHP << endl;
  }
  
//  cout << endl;
  
  ofstream ofs(outfile.c_str(), ios_base::binary);
  ofs.write(fileUS, sizeUS);
  
  return 0;
}
