#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "util/ByteConversion.h"
#include "util/TSerialize.h"

using namespace std;
using namespace BlackT;

void printCompared(double first, double second) {
  
  int positive = (first >= 0);

  cout << " | align=\"center\" | ";
  if (first == second) {
      cout.setf(ios::fixed, ios::floatfield);
      cout << dec << setprecision(0);
      cout << "<div style=\"color: gray;\">" << first << "</div>\n";
  }
  else {
    cout << "<div style=\"font-weight: bold;\">";
    cout.setf(ios::fixed, ios::floatfield);
    cout << dec << setprecision(0);
    cout << first << " -> " << second;
  
    if (positive) {
      if (second > first) {
        cout << "<div style=\"color: red;\">";
      }
      else {
        cout << "<div style=\"color: green;\">";
      }
      cout << dec << ((second > first) ? "+" : "") << second - first << ", ";
//      cout << ((second - first) / first) * 100 << "%\n";
      cout << dec << setprecision(2);
      cout << "×" << second / first << "\n";
    }
    else {
      if (second > first) {
        cout << "<div style=\"color: green;\">";
      }
      else {
        cout << "<div style=\"color: red;\">";
      }
      cout << dec << ((second > first) ? "+" : "") << second - first << ", ";
      cout << dec << setprecision(2);
      cout << "×" << second / first << "\n";
    }
    
    cout << "</div></div>\n";
  }
}

struct LoadedFile {
  char* buffer;
  int size;
};

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

LoadedFile loadFile(const char* filename) {
  LoadedFile result = { NULL, 0 };
  ifstream ifs(filename, ios_base::binary);
  int sz = fsize(ifs);
  result.buffer = new char[sz];
  result.size = sz;
  ifs.read(result.buffer, sz);
  return result;
}

struct CF2Enemy {
  
  int hp;
  int attack;
  int defense;
  int a;
  int b;
  string name;
  
  int readStats(const char* src) {
    int pos = 0;
    
    hp = TSerialize::readInt(
      src, pos, 2, EndiannessTypes::little, SignednessTypes::nosign);
    attack = TSerialize::readInt(
      src, pos, 2, EndiannessTypes::little, SignednessTypes::nosign);
    defense = TSerialize::readInt(
      src, pos, 2, EndiannessTypes::little, SignednessTypes::nosign);
    a = TSerialize::readInt(
      src, pos, 2, EndiannessTypes::little, SignednessTypes::nosign);
    b = TSerialize::readInt(
      src, pos, 2, EndiannessTypes::little, SignednessTypes::nosign);
    
    return pos;
  }
  
  int readName(const char* src) {
    int pos = 0;
    
    while (src[pos++] != 0);
    name = string(src, pos - 1);
    
    return pos;
  }
  
};
    
void compareCF2Enemy(CF2Enemy jp, CF2Enemy us) {
  cout << " |-\n";
  cout << " | align=\"center\" style=\"font-weight: bold;\" | "
    << us.name << "\n";
  printCompared(jp.hp, us.hp);
  printCompared(jp.attack, us.attack);
  printCompared(jp.defense, us.defense);
//  printCompared(jp.a, us.a);
//  printCompared(jp.b, us.b);
}

int main(int argc, char* argv[]) {
  
  if (argc < 4) return 0;
  
  LoadedFile statsUS = loadFile(argv[1]);
  LoadedFile statsJP = loadFile(argv[2]);
  LoadedFile namesUS = loadFile(argv[3]);
  
  int statsUSPos = 0;
  int statsJPPos = 0;
  int namesUSPos = 0;
  
  vector<CF2Enemy> enemiesJP;
  vector<CF2Enemy> enemiesUS;
  
  while (namesUSPos < namesUS.size) {
    CF2Enemy nextJP;
    CF2Enemy nextUS;
    
    statsUSPos += nextUS.readStats(statsUS.buffer + statsUSPos);
    namesUSPos += nextUS.readName(namesUS.buffer + namesUSPos);
    
    statsJPPos += nextJP.readStats(statsJP.buffer + statsJPPos);
    
    enemiesJP.push_back(nextJP);
    enemiesUS.push_back(nextUS);
  }
  
  cout << "{| class=\"wikitable\" style=\"margin: 1em auto 1em auto;\"\n"
          " |+ \'\'Cosmic Fantasy 2\'\' Boss Stat Changes\n"
          " ! Name\n"
          " ! HP\n"
          " ! Attack\n"
          " ! Defense\n";
//          " ! A\n"
//          " ! B\n";
  
  for (int i = 0; i < enemiesUS.size(); i++) {
    CF2Enemy jp = enemiesJP[i];
    CF2Enemy us = enemiesUS[i];
    
    if ((jp.hp != us.hp)
        || (jp.attack != us.attack)
        || (jp.defense != us.defense)
        || (jp.a != us.a)
        || (jp.b != us.b)) {
      compareCF2Enemy(jp, us);
    }
  }
  
  cout << " |}\n";
  
  return 0;
}
