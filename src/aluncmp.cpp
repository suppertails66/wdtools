#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"

using namespace std;
using namespace BlackT;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

struct EnemyEntry {
  const static int numVulnerabilities = 0x0B;
  const static int base = 0x02;
  const static int vulnerabilityBase = 0x03;

  string stats;
  
  int hp() {
    return ((const unsigned char*)(stats.c_str()))[base + 0];
  }
  
  int attack() {
    return ((const unsigned char*)(stats.c_str()))[stats.size() - 1];
  }
  
  unsigned char vulnerability(int index) {
    return ((const unsigned char*)(stats.c_str()))[vulnerabilityBase + index];
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

bool comparisonNeeded(EnemyEntry jp, EnemyEntry us) {
  if ((jp.hp() != us.hp())
      || (jp.attack() != us.attack())
      ) {
    return true;
  }
  
  for (int i = 0; i < EnemyEntry::numVulnerabilities; i++) {
    if (jp.vulnerability(i) != us.vulnerability(i)) {
      return true;
    }
  }
  
  return false;
}
/*    - bits 6-7: vulnerability settings?
      - 00 = normal damage?
      - 10 = invulnerable?
      - 01 = instakill?
      - 11 = every hit does 1hp?
    
    - bits 4-5: damage multiplier?
      - 00 = every hit does 1hp?
      - 01 = normal damage?
      - 10 = hits do double damage?
      - 11 = hits do triple damage?
      
    - bits 0-3: damage bonus per hit?
      - add this after multiplying by bits 4-5 
      
    - 10 = normal, 40 = instakill? 01 = 1 hp per hit?
    - bit 7 = no vulnerability if set (and bit 6 not set?)?
    - bit 6 = attacks instakill?
    - bit 5 = set to double damage?
    - bit 4 = normal damage? */

string getVulnerabilitySettings(unsigned char vuln) {
  int settings = (vuln & 0xC0) >> 6;
  switch (settings) {
  case 0: return "N"; break;  // normal
  case 1: return "O"; break;  // instakill
  case 2: return "-"; break;  // invulnerable
  case 3: return "X"; break;  // always 1hp
  default: return "ASDFDSA"; break;
  }
}

int getVulnerabilityMultiplier(unsigned char vuln) {
  return (vuln & 0x30) >> 4;
}

int getVulnerabilityBonus(unsigned char vuln) {
  return (vuln & 0x0F);
}

void compareVulnerability(unsigned char jp, unsigned char us, bool sep) {

  // Separator
  
  if (sep) {
    cout << " | style=\"padding: 1px 1px 1px 1px; background-color: gray;\" | \n";
  }

  // Settings
  
  cout << " | align=\"center\" style=\"";
  string settingsJP = getVulnerabilitySettings(jp);
  string settingsUS = getVulnerabilitySettings(us);
  
  if (settingsJP.compare(settingsUS) == 0) {
    cout << "color: gray; ";
  }
  else {
    cout << "color: red; font-weight: bold; ";
  }
  
  cout << "\" | ";
  
  if (settingsJP.compare(settingsUS) == 0) {
    cout << settingsJP;
  }
  else {
    cout << settingsJP << " -> " << settingsUS;
  }
  
  // Multiplier
  
  cout << "\n | ";
  
  int multiplierJP = getVulnerabilityMultiplier(jp);
  int multiplierUS = getVulnerabilityMultiplier(us);
  cout << " style=\"text-align: center;\" | ";
  if (multiplierJP == multiplierUS) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << multiplierJP
            << "x</span>";
  }
  else {
    if (multiplierJP > multiplierUS) {
      cout << "<span style=\"color: red;\">";
    }
    else {
      cout << "<span style=\"color: green;\">";
    }
      
    printStat(multiplierJP);
    
    cout    << "x -> ";
    printStat(multiplierUS);
    cout << "x";
//    cout    << "\n\n";
//    printChange(multiplierJP, multiplierUS, 999999, true);
    cout << "</span>";
  }
  
  // Bonus
  
  cout << "\n | ";
  
  int bonusJP = getVulnerabilityBonus(jp);
  int bonusUS = getVulnerabilityBonus(us);
  cout << " style=\"text-align: center;\" | ";
  if (bonusJP == bonusUS) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << bonusJP
            << "</span>";
  }
  else {
    if (bonusJP > bonusUS) {
      cout << "<span style=\"color: red;\">";
    }
    else {
      cout << "<span style=\"color: green;\">";
    }
    
    printStat(bonusJP);
    cout    << " -> ";
    printStat(bonusUS);
    cout << "</span>";
  }
  
  // End
  
  cout << "\n";
  
}

void compareEnemy(EnemyEntry jp, EnemyEntry us, int index) {
  if (!comparisonNeeded(jp, us)) return;

  cout << " |-\n";
  cout << " | align=\"center\" | {{hex|" << hex << uppercase << setw(3) << setfill('0') << index
       << "}}\n";
  printAsDiff(jp.hp(), us.hp(), 0.50);
  printAsDiff(jp.attack(), us.attack(), 0.50);
//  for (int i = 0; i < EnemyEntry::numVulnerabilities; i++) {
  compareVulnerability(jp.vulnerability(7), us.vulnerability(7), false);
  for (int i = 8; i < EnemyEntry::numVulnerabilities; i++) {
    compareVulnerability(jp.vulnerability(i), us.vulnerability(i), true);
  }
  
//  printAsDiff(jp.a, us.a, 0.50);
/*  printAsDiff(jp.d, us.d, 0.50);
  printAsDiff(jp.b, us.b, 0.50);
  printAsDiff(jp.c, us.c, 0.50);
  printAsDiff(jp.e, us.e, -0.50, true);
  printAsDiff(jp.f, us.f, -0.50, true); */
//  printAsDiff(jp.exp, us.exp, -0.50, true);
//  printAsDiff(jp.gold, us.gold, -0.50, true);
}


int main(int argc, char* argv[]) {
  if (argc < 4) {
    return 0;
  }
  
  ifstream ifsJP(argv[2], ios_base::binary);
  ifstream ifsUS(argv[1], ios_base::binary);
  int index = TStringConversion::fromString<int>(string(argv[3]));
  
  int sizeJP = fsize(ifsJP);
  int sizeUS = fsize(ifsUS);
  
  char* fileJP = new char[sizeJP];
  char* fileUS = new char[sizeUS];
  
  ifsJP.read(fileJP, sizeJP);
  ifsUS.read(fileUS, sizeUS);
  
  if (sizeJP != sizeUS) {
    cerr << "Size mismatch!: " << argv[1] << " " << argv[2] << endl;
    return 0;
  }
  
  if ((sizeJP < 0xF) || (sizeUS < 0xF)) {
    return 0;
  } 
  
  EnemyEntry jp;
  EnemyEntry us;
  
  jp.stats = string(fileJP, sizeJP);
  us.stats = string(fileUS, sizeUS);
  
  compareEnemy(jp, us, index);
  
  delete fileJP;
  delete fileUS;
  
  return 0;
}
