#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "util/TStringConversion.h"
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

bool tryPrintCharacter(unsigned char first, string& dst, bool& textmode,
                       int pos) {
  unsigned char firstlow = first & 0x7F;
  if (first == 0xFF) {
    textmode = false;
  }
  else {
    if ((firstlow == 0) || (firstlow >= 0x60)) {
      cerr << hex << pos << ": Invalid character " << (int)firstlow << endl;
      return false;
    }
    else {
      dst += (char)(firstlow + 0x1F);
      if (first & 0x80) textmode = false;
    }
  }
  
  return true;
}

bool readL2ebString(unsigned char* src, int srcsize, int& posR,
                    string& dst) {
  int pos = posR;
  
  unsigned char first = src[pos + 1];
  unsigned char second = src[pos];
  
  switch (first & 0xF0) {
  case 0xB0:
  case 0xA0:
    
    break;
  default:
    if (first == 0x06) {
    
    }
    else {
      return false;
    }
    break;
  }
  
  int chnglen = (srcsize & -2);
  bool textmode = false;
  bool done = false;
  int charactersPrinted = 0;
  for (int i = pos; i < chnglen; i += 2) {
    int next = ByteConversion::fromBytes(
      src + i, 2, EndiannessTypes::little, SignednessTypes::nosign);
    
    if (textmode == true) {
      first = src[i + 1];
      second = src[i];
      
//      unsigned char firstlow = first & 0x7F;
//      unsigned char secondlow = second & 0x7F;
      
      if (!tryPrintCharacter(first, dst, textmode, i + 1)) return false;
      if (!tryPrintCharacter(second, dst, textmode, i)) return false;
      
      charactersPrinted += 2;
      
/*      if (first == 0xFF) {
        textmode = false;
      }
      else {
        if (firstlow >= 0x61) {
          cerr << hex << i << ": Invalid character " << (int)firstlow << endl;
          return false;
        }
        else {
          dst += (char)(firstlow + 0x1F);
          if (first & 0x80) textmode = false;
        }
      }
      
      if (second == 0xFF) {
        textmode = false;
      }
      else {
        if (secondlow >= 0x61) {
          cerr << hex << i << ": Invalid character " << (int)secondlow << endl;
          return false;
        }
        else {
          dst += (char)(secondlow + 0x1F);
          if (second & 0x80) textmode = false;
        }
      } */
      
    }
    else {
      int command = next & 0xFF00;
      switch (command) {
      // end of message?
//      case 0x0000:
//        dst += "[END]\n\n";
//        done = true;
//        break;
      // resume text
      case 0x0600:
        textmode = true;
        if (!tryPrintCharacter((next & 0xFF), dst, textmode, i)) return false;
        break;
      // dialogue choice?
      case 0x0100:
        dst += "[0100]\n\n";
        break;
      // linebreak?
      case 0x1000:
        dst += "\n";
        break;
      // pause for input
      case 0x2000:
        dst += "[BRK]\n\n";
        break;
      // ? regularly occurs at the end of lines, in middle of box
      case 0x3000:
//        dst += "[BRK2]\n";
        break;
      // dialogue choice?
      case 0x4000:
        dst += "[4000]\n";
        break;
      // ?
      case 0x5000:
        dst += "[5000]\n";
        break;
      // ?
      case 0x6000:
        dst += "[6000]\n";
        break;
      // ?
      case 0xD000:
        dst += "[D000]\n";
        break;
      default:
        // try short commands
        switch (next & 0xF000) {
        // end of message?
        case 0x0000:
          dst += "[END]\n\n";
          done = true;
          break;
        case 0xA000:
          dst += string("[SPECIAL_A_")
            + TStringConversion::toString(next & 0xFFF) + "]\n";
          break;
        case 0xB000:
          dst += string("[POR")
            + TStringConversion::toString(next & 0xFFF) + "]\n";
          break;
        // failure
        default:
          cerr << hex << i
            << ": Unknown control section command " << hex << command << endl;
          return false;
          break;
        }
        break;
      }
    }
    
    if (done) {
      posR = i;
      break;
    }
  }

  if (!done) {
    return false;
  }
  
  if (charactersPrinted < 4) return false;
  
  return true;
}

int main(int argc, char* argv[]) {
  if (argc < 3) return 0;
  
  ifstream ifs(argv[1], ios_base::binary);
  int sz = fsize(ifs);
  char* buffer = new char[sz];
  ifs.read(buffer, sz);
  
  int chnglen = (sz & -2);
  for (int i = 0; i < chnglen; i += 2) {
    unsigned char first = buffer[i + 1] & 0x7F;
    unsigned char second = buffer[i] & 0x7F;
    first += 0x1F;
    second += 0x1F;
    buffer[i] = first;
    buffer[i + 1] = second;
  }
  
  ofstream ofs(argv[2], ios_base::binary);
  ofs.write(buffer, sz); 
  
/*  int chnglen = (sz & -2);
  int pos = 0;
  while (pos < chnglen) {
    string str;
    int oldpos = pos;
    if (readL2ebString((unsigned char*)buffer, sz, pos, str)) {
      cout << "===" << hex << oldpos << "===\n";
      cout << str;
    }
    else {
      pos += 2;
    }
  } */
  
  delete buffer;
  
  return 0;
}
