#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cmath>
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

char* readFile(ifstream& ifs) {
  int sz = fsize(ifs);
  char* buffer = new char[sz];
  ifs.read(buffer, sz);
  return buffer;
}

const static char initstr[]
  = "\xA9\xFF"
    "\x9D\x86\x34"
    "\xA9\xFF"
    "\x9D\xE6\x34"
    "\xA9\xFF"
    "\x9D\x16\x35"
    "\xA9\xFF"
    "\x9D\xB6\x34"
    "\xA9\xFF"
    "\x9D\x46\x35"
    "\xA9\xFF"
    "\x9D\x76\x35";
int initstrSize = strlen(initstr);

const static char initstr2[]
  = "\xA9\x03"
    "\x9D\xA3\x2B"
    "\x8A\xA6"
    "\x31\x9D"
    "\x46\x32"
    "\xA9\xFF"
    "\x85\x02"
    "\xA9\xFF"
    "\x85\x03"
    "\x20\xFF\xFF"
    "\x20\xFF\xFF";
int initstr2Size = strlen(initstr2);
    
bool matchesInit(char* src, const char* str, bool d = false) {
  int pos = 0;
  while (str[pos] != 0) {
//    if (d) {
//      cout << (int)(initstr[pos]) << " " << (int)(src[pos]) << endl;
//    }
  
    if ((unsigned char)(str[pos]) == 0xFF) {
      ++pos;
      continue;
    }
    
    if (src[pos] != str[pos]) {
//      if (d) { char c; cin >> c; }
      return false;
    }
    
    ++pos;
  }
  
//  if (d) { char c; cin >> c; }
  
  return true;
}

void generateInitOffsets(char* buffer, int sz, vector<int>& results) {
  int checkSize = sz - initstrSize;
  
  for (int i = 0; i < checkSize; i++) {
    if (matchesInit(buffer + i, initstr)) {
      results.push_back(i);
    }
  }
}

struct EnemyInit {
  
  int offset;
  int a;  // ? always 80?
  int b;  // attack
  int c;  // defense?
  int d;  // hp
  int e;  // exp
  int f;  // gold
  
  void read(char* src) {
    offset = 0;
    a = ByteConversion::fromBytes(src + (5 * 0) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    b = ByteConversion::fromBytes(src + (5 * 1) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    c = ByteConversion::fromBytes(src + (5 * 2) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    d = ByteConversion::fromBytes(src + (5 * 3) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    e = ByteConversion::fromBytes(src + (5 * 4) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    f = ByteConversion::fromBytes(src + (5 * 5) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
  }
  
  void readShort(char* src) {
    offset = 0;
    a = 0;
    b = ByteConversion::fromBytes(src + 0,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    c = ByteConversion::fromBytes(src + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    d = ByteConversion::fromBytes(src + 2,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    e = ByteConversion::fromBytes(src + 3,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    f = ByteConversion::fromBytes(src + 4,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
  }
  
  void write(char* dst) {
    ByteConversion::toBytes(a, dst + (5 * 0) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    ByteConversion::toBytes(b, dst + (5 * 1) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    ByteConversion::toBytes(c, dst + (5 * 2) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    ByteConversion::toBytes(d, dst + (5 * 3) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    ByteConversion::toBytes(e, dst + (5 * 4) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
    ByteConversion::toBytes(f, dst + (5 * 5) + 1,
                                  1,
                                  EndiannessTypes::little,
                                  SignednessTypes::nosign);
  }
  
  void print(ostream& ofs) {
    ofs << "a: " << a << endl;
    ofs << "b: " << b << endl;
    ofs << "c: " << c << endl;
    ofs << "d: " << d << endl;
    ofs << "e: " << e << endl;
    ofs << "f: " << f << endl;
  }
  
};

void buildEnemyInitTable(char* buffer,
                         vector<int> offsets,
                         vector<EnemyInit>& results) {
  for (int i = 0; i < offsets.size(); i++) {
    EnemyInit enemy;
    enemy.read(buffer + offsets[i]);
    enemy.offset = offsets[i];
    results.push_back(enemy);
  }
}

const static int entriesPerStatTableSize = 30;
const static int entriesPerStatTable[entriesPerStatTableSize] = {
  // 0
  1,
  1,
  1,
  2,
  
  // 4
  4,
  4,
  
  // 6
  1,
  1,
  2,
  1,
  
  // A
  1,
  1,
  11,
  1,
  1,
  
  // repeats of earlier tables
  // 0
  1,
  1,
  1,
  2,
  
  // 4
  4,
  4,
  
  // 6
  1,
  1,
  2,
  1,
  
  // A
  1,
  1,
  11,
  1,
  1
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

void compareEnemy(EnemyInit jp, EnemyInit us) {
//  printAsDiff(jp.a, us.a, 0.50);
  printAsDiff(jp.d, us.d, 0.50);
  printAsDiff(jp.b, us.b, 0.50);
  printAsDiff(jp.c, us.c, 0.50);
  printAsDiff(jp.e, us.e, -0.50, true);
  printAsDiff(jp.f, us.f, -0.50, true);
//  printAsDiff(jp.exp, us.exp, -0.50, true);
//  printAsDiff(jp.gold, us.gold, -0.50, true);
}

bool comparisonNeeded(EnemyInit jp, EnemyInit us) {
  if ((jp.b == us.b)
      && (jp.c == us.c)
      && (jp.d == us.d)
      && (jp.e == us.e)
      && (jp.f == us.f)) {
    return false;
  }
  
  return true;
}

void compareEnemies(vector<EnemyInit>& enemies_jp,
                    vector<EnemyInit>& enemies_us) {
  cout << "{| class=\"wikitable\" style=\"margin: 1em auto 1em auto;\"\n"
       << " ! width=\"100px\" | Data Offsets \n"
       << " ! width=\"100px\" | HP \n"
       << " ! width=\"100px\" | Attack \n"
       << " ! width=\"100px\" | Defense \n"
       << " ! width=\"100px\" | EXP \n"
       << " ! width=\"100px\" | Gold \n";
//       << " |-\n";
  for (int i = 0; i < enemies_jp.size() / 2; i++) {
    
    if (!comparisonNeeded(enemies_jp[i], enemies_us[i])) {
      continue;
    }
    
    cout << " |-\n";
    cout << " | style=\"text-align: center; font-weight: bold;\""
            << " | JP: {{hex|" << hex << uppercase
            << enemies_jp[i].offset << "}}\n\n"
            << "US: {{hex|" << hex << uppercase
            << enemies_us[i].offset << "}}\n";
//    cout << " | style=\"text-align: center; font-weight: bold;\""
//            << " | " << enemies_us[i].name << "\n";
    compareEnemy(enemies_jp[i], enemies_us[i]);
  }
  cout << " |}\n\n";
}

void printCSV(vector<EnemyInit>& enemies_jp,
                    vector<EnemyInit>& enemies_us) {
  for (int i = 0; i < enemies_jp.size() / 2; i++) {
    cout << enemies_jp[i].a << ","
         << enemies_jp[i].b << ","
         << enemies_jp[i].c << ","
         << enemies_jp[i].d << ","
         << enemies_jp[i].e << ","
         << enemies_jp[i].f << ","
         << enemies_jp[i].offset << ","
         << ","
         << enemies_us[i].a << ","
         << enemies_us[i].b << ","
         << enemies_us[i].c << ","
         << enemies_us[i].d << ","
         << enemies_us[i].e << ","
         << enemies_us[i].f << ","
         << enemies_us[i].offset << endl;
  }
}

int main(int argc, char* argv[]) {
  
  if (argc < 3) {
    
    return 0;
  }
  
  ifstream ifs(argv[1], ios_base::binary);
  int szUS = fsize(ifs);
  char* bufferUS = readFile(ifs);
  ifs.close();
  ifs.open(argv[2], ios_base::binary);
  int szJP = fsize(ifs);
  char* bufferJP = readFile(ifs);
  ifs.close();
  
  vector<int> inits2US;
  for (int i = 0; i < szUS - initstr2Size; i++) {
    if (matchesInit(bufferUS + i, initstr2)
        // these two won't match due to ??? some change
        || (i == 0x10115e)
        || (i == 0xb0215e)) {
//      cout << hex << i << endl;
      inits2US.push_back(i);
    }
  }
//  cout << endl;
  
  vector<int> inits2JP;
  for (int i = 0; i < szJP - initstr2Size; i++) {
    if (matchesInit(bufferJP + i, initstr2)) {
//      cout << hex << i << endl;
      inits2JP.push_back(i);
    }
  }
//  cout << endl;

  // these were deleted from the US release??
  // 10115e
  // b0215e
//  inits2JP.erase(inits2JP.begin() + 0);
//  inits2JP.erase(inits2JP.begin() + 14);
  
//  cout << inits2US.size() << endl;
//  cout << inits2JP.size() << endl;


/*  vector<EnemyInit> enemiesJP;
  vector<EnemyInit> enemiesUS;
  for (int i = 0; i < inits2US.size(); i++) {
    char addrUS[2];
    char addrJP[2];
    addrUS[0] = *(bufferUS + inits2US[i] + 12);
    addrUS[1] = *(bufferUS + inits2US[i] + 16);
    addrJP[0] = *(bufferJP + inits2JP[i] + 12);
    addrJP[1] = *(bufferJP + inits2JP[i] + 16);
    
    int realAddrUS = ByteConversion::fromBytes(addrUS, 2,
                                               EndiannessTypes::little,
                                               SignednessTypes::nosign);
    int realAddrJP = ByteConversion::fromBytes(addrJP, 2,
                                               EndiannessTypes::little,
                                               SignednessTypes::nosign);
    
    int convertedUS = (realAddrUS % 0x2000)
        + ((inits2US[i] / 0x2000) * 0x2000)
        + 0x1000;
    int convertedJP = (realAddrJP % 0x2000)
        + ((inits2JP[i] / 0x2000) * 0x2000)
        + 0x1000;
    
    // copy all entries
//    memcpy(bufferUS + convertedUS,
//           bufferJP + convertedJP,
//           entriesPerStatTable[i] * 5);

    for (int j = 0; j < entriesPerStatTable[i]; j++) {
      EnemyInit enemyJP;
      EnemyInit enemyUS;
      enemyJP.readShort(bufferJP + convertedJP + (j * 5));
      enemyUS.readShort(bufferUS + convertedUS + (j * 5));
      enemyJP.offset = convertedJP + (j * 5);
      enemyUS.offset = convertedUS + (j * 5);
      enemiesJP.push_back(enemyJP);
      enemiesUS.push_back(enemyUS);
    }
    
//    if (enemiesJP.size() == 0) {
//      continue;
//    }
    
//    compareEnemies(enemiesJP, enemiesUS);
  }
  printCSV(enemiesJP, enemiesUS);
//  compareEnemies(enemiesJP, enemiesUS); */
  
//  ofstream ofs((string(argv[1]) + "_fix").c_str(),
//               ios_base::binary | ios_base::trunc);
//  ofs.write(bufferUS, szUS);
  
  
  
  
/*  vector<int> offsetsUS;
  vector<int> offsetsJP;
  
  generateInitOffsets(bufferUS, szUS, offsetsUS);
  generateInitOffsets(bufferJP, szJP, offsetsJP);
  
  vector<EnemyInit> enemiesUS;
  vector<EnemyInit> enemiesJP;
  
  buildEnemyInitTable(bufferUS, offsetsUS, enemiesUS);
  buildEnemyInitTable(bufferJP, offsetsJP, enemiesJP);
  
  printCSV(enemiesJP, enemiesUS);
//  compareEnemies(enemiesJP, enemiesUS);  */
  
/*  for (int i = 0; i < enemiesUS.size(); i++) {
//    cout << "Entry " << i << ": "
//      << hex << offsetsJP[i] << " / " << hex << offsetsUS[i] << endl;
    
//    cout << "?:    " << enemiesJP[i].a << " -> " << enemiesUS[i].a << endl;
//    cout << "atk:  " << enemiesJP[i].b << " -> " << enemiesUS[i].b << endl;
//    cout << "def:  " << enemiesJP[i].c << " -> " << enemiesUS[i].c << endl;
//    cout << "hp:   " << enemiesJP[i].d << " -> " << enemiesUS[i].d << endl;
//    cout << "exp:  " << enemiesJP[i].e << " -> " << enemiesUS[i].e << endl;
//    cout << "gold: " << enemiesJP[i].f << " -> " << enemiesUS[i].f << endl;
//    cout << endl; 
    
    enemiesUS[i].a = enemiesJP[i].a;
    enemiesUS[i].b = enemiesJP[i].b;
    enemiesUS[i].c = enemiesJP[i].c;
    enemiesUS[i].d = enemiesJP[i].d;
    enemiesUS[i].e = enemiesJP[i].e;
    enemiesUS[i].f = enemiesJP[i].f;
    enemiesUS[i].write(bufferUS + offsetsUS[i]);
    
//    if (enemiesUS[i].a != 0x80) cerr << i << endl;
  } */
  
//  ofstream ofs((string(argv[1]) + "_fix").c_str(),
//               ios_base::binary | ios_base::trunc);
//  ofs.write(bufferUS, szUS); 
  
/*  int total = 0;
  for (int i = 0; i < checkSize; i++) {
//    if (i == 0x1C18E5) {
//      matchesInit(buffer + i, true);
//    }
//    else
    if (matchesInit(buffer + i)) {
      cout << hex << i << endl;
      ++total;
    }
  }
  cout << endl << "Total: " << total << endl; */
  
  vector<EnemyInit> enemiesUS;
  vector<EnemyInit> enemiesJP;
  enemiesJP.push_back(
    EnemyInit { 0x00141100, 0x00, 0x28, 0x0c, 0xfa, 0xE8, 0x20 } );
  enemiesJP.push_back(
    EnemyInit { 0x001C1857, 0x00, 0x36, 0x16, 0xfa, 0xD0, 0x2C } );
  enemiesJP.push_back(
    EnemyInit { 0x00202B63, 0x00, 0x46, 0x1f, 0x1f, 0x9d, 0x00 } );
  enemiesJP.push_back(
    EnemyInit { 0x002031e0, 0x00, 0x46, 0x1f, 0xfa, 0xb8, 0x90 } );
  enemiesJP.push_back(
    EnemyInit { 0x002815C3, 0x00, 0x3c, 0x14, 0xe6, 0xf4, 0xc8 } );
  enemiesJP.push_back(
    EnemyInit { 0x00281839, 0x00, 0x48, 0x23, 0xfa, 0xa0, 0x58 } );
  enemiesJP.push_back(
    EnemyInit { 0x00281de7, 0x00, 0x48, 0x23, 0xfa, 0xa0, 0x58 } );
  enemiesJP.push_back(
    EnemyInit { 0x00305018, 0x00, 0x78, 0x3c, 0xff, 0x00, 0x00 } );
  enemiesJP.push_back(
    EnemyInit { 0x00305126, 0x00, 0x3c, 0x28, 0x32, 0x00, 0x00 } );
  enemiesJP.push_back(
    EnemyInit { 0x003051ac, 0x00, 0x3c, 0x28, 0x32, 0x00, 0x00 } );
  enemiesJP.push_back(
    EnemyInit { 0x003225de, 0x00, 0x4b, 0x28, 0xfa, 0x0a, 0x64 } );
  
  enemiesUS.push_back(
    EnemyInit { 0x00141100, 0x00, 0x5f, 0x1b, 0xfa, 0xc8, 0x96 } );
  enemiesUS.push_back(
    EnemyInit { 0x001C18D3, 0x00, 0x6e, 0x1c, 0xdc, 0x40, 0x96 } );
  enemiesUS.push_back(
    EnemyInit { 0x00202BC1, 0x00, 0x64, 0x2d, 0x1f, 0x9d, 0x00 } );
  enemiesUS.push_back(
    EnemyInit { 0x0020323E, 0x00, 0x78, 0x2d, 0xfa, 0xb8, 0x90 } );
  enemiesUS.push_back(
    EnemyInit { 0x002815C9, 0x00, 0x8c, 0x24, 0xe6, 0x2c, 0x64 } );
  enemiesUS.push_back(
    EnemyInit { 0x0028183F, 0x00, 0x64, 0x2d, 0xfa, 0xac, 0x5e } );
  enemiesUS.push_back(
    EnemyInit { 0x00281ded, 0x00, 0x63, 0x28, 0xfa, 0xd8, 0x7c } );
  enemiesUS.push_back(
    EnemyInit { 0x00305018, 0x00, 0x8c, 0x8c, 0xff, 0x00, 0x00 } );
  enemiesUS.push_back(
    EnemyInit { 0x00305126, 0x00, 0xfa, 0x2b, 0x32, 0x00, 0x00 } );
  enemiesUS.push_back(
    EnemyInit { 0x003051ac, 0x00, 0xfa, 0x2b, 0x32, 0x00, 0x00 } );
  enemiesUS.push_back(
    EnemyInit { 0x0032265c, 0x00, 0x6e, 0x32, 0xfa, 0x0a, 0x64 } );
  
  enemiesJP.resize(enemiesJP.size() * 2);
  enemiesUS.resize(enemiesJP.size() * 2);
  compareEnemies(enemiesJP, enemiesUS);
  
  delete bufferUS;
  delete bufferJP;
  
  return 0;
}
