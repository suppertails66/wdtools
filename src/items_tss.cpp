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

struct ItemStats {

  const static int size = 0x8;
  
  int address;
  
  int price;
  int a;
  int b;
  int c;
  int d;
  int e;
  int attack;
  
  int load(const char* src, int offset) {
    address = offset;
    price = TSerialize::readInt(src, offset,
                                    2,
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
    attack = TSerialize::readInt(src, offset,
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
    TSerialize::writeIntS(dst, offset, price, 2,
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
    TSerialize::writeIntS(dst, offset, attack, 1,
                         EndiannessTypes::big,
                         SignednessTypes::nosign);
    return size;
  }
  
/*  ItemStats& operator=(const ItemStats& c) {
    initialHP = c.initialHP;
    hp = c.hp;
    initialMP = c.initialMP;
    mp = c.mp;
    
    return *this;
  } */
  
};

void readItems(char* src, int offset, int number, vector<ItemStats>& dst) {
  for (int i = 0; i < number; i++) {
    ItemStats stats;
    offset += stats.load(src, offset);
    dst.push_back(stats);
  }
}

void printIfChanged(string name, int us, int jp) {
  if (us != jp) {
    cout << name << ": " << dec << jp << " -> " << dec << us << endl;
  }
}

void compareItem(const ItemStats& us, const ItemStats& jp) {
  printIfChanged("Price", us.price, jp.price);
  printIfChanged("A", us.a, jp.a);
  printIfChanged("B", us.b, jp.b);
  printIfChanged("C", us.c, jp.c);
  printIfChanged("D", us.d, jp.d);
  printIfChanged("E", us.e, jp.e);
  printIfChanged("Attack", us.attack, jp.attack);
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
  
  int itemsOffsetUS = 0x195D8;
  int itemsOffsetJP = 0x1926E;
  
  vector<ItemStats> itemsUS;
  vector<ItemStats> itemsJP;
  
  readItems(fileUS, itemsOffsetUS, 0x88, itemsUS);
  readItems(fileJP, itemsOffsetJP, 0x88, itemsJP);
  
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
  
//  cout << "Enemy count: " << itemsUS.size() << endl;
  for (unsigned int i = 0; i < itemsUS.size(); i++) {
//    cout << itemsUS[i].address << " " << itemsJP[i].address << endl;

    cout << "Item " << i << endl;
    compareItem(itemsUS[i], itemsJP[i]);

    int offset = itemsUS[i].address;
    itemsJP[i].save(fileUS, offset);
  }
  
//  cout << endl;
  
  ofstream ofs(outfile.c_str(), ios_base::binary);
  ofs.write(fileUS, sizeUS);
  
  return 0;
}
