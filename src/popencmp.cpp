#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
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

struct EnemyTableEntry {
  int hp;
  int attack;
  int defense;
  int money;
  
  void read(ifstream& ifs) {
    char buffer[2];
  
    // ?
    ifs.get();
    // ?
    ifs.get();
    ifs.read(buffer, 2);
    hp = ByteConversion::fromBytes(buffer, 2,
                                   EndiannessTypes::big,
                                   SignednessTypes::nosign);
    ifs.read(buffer, 2);
    attack = ByteConversion::fromBytes(buffer, 2,
                                   EndiannessTypes::big,
                                   SignednessTypes::nosign);
    ifs.read(buffer, 2);
    defense = ByteConversion::fromBytes(buffer, 2,
                                   EndiannessTypes::big,
                                   SignednessTypes::nosign);
    ifs.read(buffer, 2);
    money = ByteConversion::fromBytes(buffer, 2,
                                   EndiannessTypes::big,
                                   SignednessTypes::nosign);
    // ?
    ifs.get();
    // ?
    ifs.get();
    // ?
    ifs.get();
    // ?
    ifs.get();
    // ?
    ifs.get();
    // ?
    ifs.get();
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

void compareEnemy(EnemyTableEntry jp, EnemyTableEntry us) {
/*  cout << '\t' << "HP:    " << jp.hp << " -> " << us.hp << endl;
  cout << '\t' << "Atk:   " << jp.attack << " -> " << us.attack << endl;
  cout << '\t' << "Def:   " << jp.defense << " -> " << us.defense << endl;
  cout << '\t' << "Money: " << jp.money << " -> " << us.money << endl; */
  
//  double hpRatio = ((double)(us.hp) / (double)(jp.hp)) - 1.00;
//  bool hpTriggered = hpRatio >= 0.50;
  cout << " | style=\"text-align: center;\" | ";
  if (jp.hp == us.hp) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << jp.hp
            << "</span>\n";
  }
  else {
    printStat(jp.hp);
    cout    << " -> ";
    printStat(us.hp);
    cout    << "\n\n";
    printChange(jp.hp, us.hp, 0.66);
    cout    << "\n";
  }
  
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
    printChange(jp.attack, us.attack, 0.66);
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
    printChange(jp.defense, us.defense, 0.66);
    cout    << "\n";
  }
  
  cout << " | style=\"text-align: center;\" | ";
  if (jp.money == us.money) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << jp.money
            << "</span>\n";
  }
  else {
    printStat(jp.money);
    cout    << " -> ";
    printStat(us.money);
    cout    << "\n\n";
    printChange(jp.money, us.money, -0.66, true);
    cout    << "\n";
  }
}

bool comparisonNeeded(EnemyTableEntry jp, EnemyTableEntry us) {
  if ((jp.hp == us.hp)
      && (jp.attack == us.attack)
      && (jp.defense == us.defense)
      && (jp.money == us.money)) {
    return false;
  }
  
  return true;
}

void compareEnemies(vector<EnemyTableEntry>& enemies_jp,
                    vector<EnemyTableEntry>& enemies_us) {
  cout << "{| class=\"wikitable\" style=\"margin: 1em auto 1em auto;\"\n"
       << " ! ID #\n"
       << " ! HP \n"
       << " ! Attack \n"
       << " ! Defense \n"
       << " ! Gold \n";
//       << " |-\n";
  for (int i = 0; i < enemies_jp.size(); i++) {
//    cout << "Enemy " << i << ": " << endl;
    if (i >= 102) {
      break;
    }
    
    if (!comparisonNeeded(enemies_jp[i], enemies_us[i])) {
      continue;
    }
    
    cout << " |-\n";
    cout << " | style=\"text-align: center; font-weight: bold;\""
            << " | {{hex|" << hex << uppercase << setw(2) << setfill('0') << i << "}}\n";
    compareEnemy(enemies_jp[i], enemies_us[i]);
  }
  cout << " |}";
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    return 0;
  }

  vector<EnemyTableEntry> enemies_jp;
  vector<EnemyTableEntry> enemies_us;
  
  ifstream ifs;
  ifs.open(argv[1], ios_base::binary);
  while (ifs.good()) {
    EnemyTableEntry e;
    e.read(ifs);
    enemies_jp.push_back(e);
  }
  
  ifs.close();
  
  ifs.open(argv[2], ios_base::binary);
  while (ifs.good()) {
    EnemyTableEntry e;
    e.read(ifs);
    enemies_us.push_back(e);
  }
  
  compareEnemies(enemies_jp, enemies_us);
}
