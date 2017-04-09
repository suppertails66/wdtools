#include <iostream>
#include <fstream>
#include <string>
#include "util/ByteConversion.h"
#include "util/TSerialize.h"

using namespace std;
using namespace BlackT;

struct LSSSEnemyStats {
  
  // 1b 0x0 = 1d2c82
  int type;
  // 1b 0x1 = 1d2c7c
  int level;
  // 2b 0x2 = 1d2c68
  int hp;
  // 2b 0x4 = 1d2c70
  int attack;
  // 2b 0x6 = 1d2c72
  int defense;
  // 2b 0x8 = 1d2c74
  int agility;
  // 2b 0xa = 1d2c76
  int wisdom;
  // 2b 0xc = 1d2c78
  int magicDefense;
  // 1b 0xe = 1d2c7b
  int range;
  // 1b 0xf = 1d2c7e = something that affects attack
  // possibly add (this number / 4) as a bonus per attack?
  int atk2;
  // 1b 0x10 = 1d2c7a = numattacks
  int numAttacks;
  // 1b 0x11 = 1d2c83 = ?
  int d;
  // 1b 0x12 = 1d2c84 = ?
  int e;
  // 1b 0x13 = 1d2c85 = ?
  int f;
  // 1b 0x14 = 1d2c86 = ?
  int g;
  // 1b 0x15 = 1d2c87 = ?
  int h;
  // 1b 0x16 = 1d2c7f = ?
  int i;
  // 1b 0x17 = 1d2c81 = ?
  int j;
  // 1b 0x18 = 1d2c87 = ?
  int k;
  // 1b 0x19 = not used?
  int l;
  // 2b 0x1a = 1d2c8c = exp?
  int exp;
  // 2b 0x1c = 1d2c8e = gold?
  int gold;
  // 1b 0x1e = 1d2c88
  int o;
  // 1b 0x1f = 1d2c89
  int p;
  // 1b 0x20 = 1d2c8a
  int q;
  // 1b 0x21 = 1d2c8b
  int r;
  // 2b 0x22 = 1d2c90
  int s;
  // 2b 0x24 = 1d2c92
  int t;
  
  void read(istream& ifs) {
    type = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    level = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    hp = TSerialize::readInt(ifs, 2, EndiannessTypes::little,
                               SignednessTypes::nosign);
    attack = TSerialize::readInt(ifs, 2, EndiannessTypes::little,
                               SignednessTypes::nosign);
    defense = TSerialize::readInt(ifs, 2, EndiannessTypes::little,
                               SignednessTypes::nosign);
    agility = TSerialize::readInt(ifs, 2, EndiannessTypes::little,
                               SignednessTypes::nosign);
    wisdom = TSerialize::readInt(ifs, 2, EndiannessTypes::little,
                               SignednessTypes::nosign);
    magicDefense = TSerialize::readInt(ifs, 2, EndiannessTypes::little,
                               SignednessTypes::nosign);
    range = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    atk2 = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    numAttacks = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    d = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    e = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    f = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    g = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    h = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    i = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    j = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    k = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    l = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    exp = TSerialize::readInt(ifs, 2, EndiannessTypes::little,
                               SignednessTypes::nosign);
    gold = TSerialize::readInt(ifs, 2, EndiannessTypes::little,
                               SignednessTypes::nosign);
    o = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    p = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    q = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    r = TSerialize::readInt(ifs, 1, EndiannessTypes::little,
                               SignednessTypes::nosign);
    s = TSerialize::readInt(ifs, 2, EndiannessTypes::little,
                               SignednessTypes::nosign);
    t = TSerialize::readInt(ifs, 2, EndiannessTypes::little,
                               SignednessTypes::nosign);
  }
  
  void compareStat(string name, int first, int second) {
    if (first != second) {
      cout << name << ":\t" << first << " -> " << second << endl;
    }
  }
  
  void compare(LSSSEnemyStats obj) {
    compareStat("Type", type, obj.type);
    compareStat("Level", level, obj.level);
    compareStat("HP", hp, obj.hp);
    compareStat("Atk", attack, obj.attack);
    compareStat("Def", defense, obj.defense);
    compareStat("Agil", agility, obj.agility);
    compareStat("Wis", wisdom, obj.wisdom);
    compareStat("MDef", magicDefense, obj.magicDefense);
    compareStat("Rng", range, obj.range);
    compareStat("Atk2", atk2, obj.atk2);
    compareStat("#Atk", numAttacks, obj.numAttacks);
    compareStat("d", d, obj.d);
    compareStat("e", e, obj.e);
    compareStat("f", f, obj.f);
    compareStat("g", g, obj.g);
    compareStat("h", h, obj.h);
    compareStat("i", i, obj.i);
    compareStat("j", j, obj.j);
    compareStat("k", k, obj.k);
    compareStat("l", l, obj.l);
    compareStat("EXP", exp, obj.exp);
    compareStat("Gold", gold, obj.gold);
    compareStat("o", o, obj.o);
    compareStat("p", p, obj.p);
    compareStat("q", q, obj.q);
    compareStat("r", r, obj.r);
    compareStat("s", s, obj.s);
    compareStat("t", t, obj.t);
  }
  
};

int main(int argc, char* argv[]) {
  
  if (argc < 3) return 0;
  
  ifstream ifsus(argv[1]);
  ifstream ifsjp(argv[2]);
  
  int num = 0;
  while (ifsus.good()) {
    ifsus.get();
    if (!ifsus.good()) break;
    ifsus.unget();
    
    LSSSEnemyStats jp;
    LSSSEnemyStats us;
    
    jp.read(ifsjp);
    us.read(ifsus);
    
    cout << "Enemy " << num << ":" << endl;
    jp.compare(us);
    cout << endl;
    
    ++num;
  }
  
  return 0;
}
