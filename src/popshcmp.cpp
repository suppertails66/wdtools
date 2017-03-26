#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <vector>
#include <map>
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

unsigned int fromBCD(unsigned int bcdValue) {
  unsigned int result = 0;
  int placeMult = 1;
  
  while (bcdValue > 0) {
    result += ((bcdValue & 0x0F) * placeMult);
    bcdValue >>= 4;
    placeMult *= 10;
  }
  
  return result;
}

unsigned int toBCD(unsigned int intValue) {
  int result = 0;
  int shift = 0;
  
  while (intValue > 0) {
    int digit = intValue % 10;
    intValue /= 10;
    
    result |= (digit << shift);
    
    shift += 4;
  }
  
  return result;
}

struct PriceTableEntry {
  int buy;
  int sell;
  
  void read(ifstream& ifs) {
    char buffer[4];
  
    ifs.read(buffer, 4);
    buy = fromBCD(ByteConversion::fromBytes(buffer, 4,
                                   EndiannessTypes::big,
                                   SignednessTypes::nosign));
    ifs.read(buffer, 4);
    sell = fromBCD(ByteConversion::fromBytes(buffer, 4,
                                   EndiannessTypes::big,
                                   SignednessTypes::nosign));
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

void comparePrice(PriceTableEntry jp, PriceTableEntry us) {
/*  cout << '\t' << "HP:    " << jp.hp << " -> " << us.hp << endl;
  cout << '\t' << "Atk:   " << jp.attack << " -> " << us.attack << endl;
  cout << '\t' << "Def:   " << jp.defense << " -> " << us.defense << endl;
  cout << '\t' << "Money: " << jp.money << " -> " << us.money << endl; */
  
//  double hpRatio = ((double)(us.hp) / (double)(jp.hp)) - 1.00;
//  bool hpTriggered = hpRatio >= 0.50;

  cout << " | style=\"text-align: center;\" | ";
  if (jp.buy == us.buy) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << jp.buy
            << "</span>\n";
  }
  else {
    printStat(jp.buy);
    cout    << " -> ";
    printStat(us.buy);
    cout    << "\n\n";
    printChange(jp.buy, us.buy, 1234.50);
    cout    << "\n";
  }
  
  cout << " | style=\"text-align: center;\" | ";
  if (jp.sell == us.sell) {
    cout << "<span style=\"color: gray;\">" << dec << setw(1) << jp.sell
            << "</span>\n";
  }
  else {
    printStat(jp.sell);
    cout    << " -> ";
    printStat(us.sell);
    cout    << "\n\n";
    printChange(jp.sell, us.sell, -1234.50, true);
    cout    << "\n";
  }
}

bool comparisonNeeded(PriceTableEntry jp, PriceTableEntry us) {
  if ((jp.buy == us.buy)
      && (jp.sell == us.sell)) {
    return false;
  }
  
  return true;
}

void compareEnemies(vector<PriceTableEntry>& prices_jp,
                    vector<PriceTableEntry>& prices_us) {
  map<int, string> itemNames;
  itemNames[0x0F] = "Amulet";
  itemNames[0x10] = "Helmet";
  itemNames[0x11] = "Stone Shoes";
  itemNames[0x12] = "Ice Shoes";
  itemNames[0x13] = "Spiked Boots";
  itemNames[0x14] = "Orange";
  itemNames[0x15] = "Cherry";
  itemNames[0x16] = "Apple";
  itemNames[0x17] = "Banana";
  itemNames[0x18] = "Melon";
  itemNames[0x1D] = "Gold Bullion";
  itemNames[0x1E] = "Elixyr";
//  itemNames[0x5D] = "???";
  
  cout << "{| class=\"wikitable\" style=\"margin: 1em auto 1em auto;\"\n"
       << " ! Item\n"
       << " ! Buy Price \n"
       << " ! Sell Price \n";
//       << " |-\n";
  for (int i = 0; i < prices_jp.size(); i++) {
//    cout << "Enemy " << i << ": " << endl;
    if (i >= 0x2E) {
      continue;
    }
    
    if (!comparisonNeeded(prices_jp[i], prices_us[i])) {
      continue;
    }
    
    cout << " |-\n";
    cout << " | style=\"text-align: center; font-weight: bold;\""
            << " | "
//            << "{{hex|"
//            << hex << uppercase << setw(2) << setfill('0')
//            << i << "}}\n";
            << itemNames[i]
//            << "}}\n";
            << "\n";
    comparePrice(prices_jp[i], prices_us[i]);
  }
  cout << " |}";
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    return 0;
  }

  vector<PriceTableEntry> prices_jp;
  vector<PriceTableEntry> prices_us;
  
  ifstream ifs;
  ifs.open(argv[1], ios_base::binary);
  while (ifs.good()) {
    PriceTableEntry e;
    e.read(ifs);
    prices_jp.push_back(e);
  }
  
  ifs.close();
  
  ifs.open(argv[2], ios_base::binary);
  while (ifs.good()) {
    PriceTableEntry e;
    e.read(ifs);
    prices_us.push_back(e);
  }
  
  compareEnemies(prices_jp, prices_us);
}
