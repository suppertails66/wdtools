#include <iostream>
#include <iomanip>
#include <ios>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"

using namespace std;
using namespace BlackT;

const static int instructionSpacing = 24;
const static int addressLength = 8;
const static int hucZpOffset = 0x2000;

bool useSplitAddresses = false;
int knownMPRs[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

class AsmOstream {
public:
  AsmOstream(ostream& ofs__)
    : ofs_(ofs__),
      caps_(false),
      baseInd_(false) { }
    
  AsmOstream& operator<<(const char* rawstr) {
    string str(rawstr);
    if (caps_) {
      for (unsigned int i = 0; i < str.size(); i++) {
        str[i] = toupper(str[i]);
      }
    }
    ofs_ << str;
    return *this;
  }
  
  template <class T>
  AsmOstream& operator<<(const T& val) {
    ofs_ << val;
    return *this;
  }
  
  ostream& internalStream() {
    return ofs_;
  }
  
  void setW(int w) {
    ofs_ << setw(w);
  }
  
  void setFill(char fill) {
    ofs_ << setfill(fill);
  }
  
  void byte(int b) {
    if (baseInd_) {
      ofs_ << "$";
    }
    
    this->setW(2);
    this->setFill('0');
    ofs_ << b;
  }
  
  void word(int w) {
    if (baseInd_) {
      ofs_ << "$";
    }
    
    this->setW(4);
    this->setFill('0');
    ofs_ << w;
  }
  
  void wordSplit(int w) {
    if (useSplitAddresses) {
      
      int mpr = (w & 0xE000) >> 13;
      int realAddr = (w & 0x1FFF);
      
      if (knownMPRs[mpr] >= 0) { 
        realAddr |= (knownMPRs[mpr] * 0x2000);
      }
      else {
//        if (caps_) {
          ofs_ << "P";
//        }
//        else {
//          ofs_ << "p";
//        }
        this->setW(1);
        this->setFill('0');
        ofs_ << mpr;
        ofs_ << ":";
      }
      
      if (baseInd_) {
        ofs_ << "$";
      }
      this->setW(4);
      this->setFill('0');
      ofs_ << realAddr;
    }
    else {
      word(w);
    }
  }
  
  void setCaps(bool caps__) {
    caps_ = caps__;
  }
  
  void setBaseInd(bool baseInd__) {
    baseInd_ = baseInd__;
  }
  
protected:
  ostream& ofs_;
  bool caps_;
  bool baseInd_;
};

int applyOffset(int offset_) {
  return offset_;
}

int getWord(char* data, SignednessTypes::SignednessType s
                          = SignednessTypes::nosign) {
  return ByteConversion::fromBytes(data, 2,
                                   EndiannessTypes::little,
                                   s);
}

int getByte(char* data, SignednessTypes::SignednessType s
                          = SignednessTypes::nosign) {
  if (s == SignednessTypes::nosign) {
    return *((unsigned char*)(data));
  }
  else {
    return ByteConversion::fromBytes(data, 1,
                                     EndiannessTypes::little,
                                     s);
  }
}


int getAddress(char* data) {
  int w = getWord(data);
  return applyOffset(w);
}

struct HucOp {
  
  int code;
  int size;
  const char* name;
  int (*handler)(HucOp, char*, int, int, AsmOstream&);
  
  int param;
};

int doOpInvalid(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  ofs << ".db  ";
  ofs.byte(op.code);
  return 1;
}

int doOpGeneric(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  ofs << op.name;
  return 1;
}

// indirect x
int doOpIx(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int arg = getByte(data + 1);
  ofs << op.name << " (";
  ofs.wordSplit(arg + hucZpOffset);
  ofs << ",X)";
  return 2;
}

// zero-page indirect
int doOpZpind(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int arg = getAddress(data + 1);
  ofs << op.name << " (";
  ofs.wordSplit(arg + hucZpOffset);
  ofs << ")";
  return 3;
}

// zero-page x
int doOpZpx(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int arg = getByte(data + 1);
  ofs << op.name;
  ofs.wordSplit(arg + hucZpOffset);
  ofs << ",X";
  return 2;
}

// absolute x
int doOpAbx(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int arg = getWord(data + 1);
  ofs << op.name << " ";
  ofs.wordSplit(arg);
  ofs << ",X";
  return 3;
}

// indirect y
int doOpIy(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int arg = getByte(data + 1);
  ofs << op.name << " (";
  ofs.wordSplit(arg + hucZpOffset);
  ofs << "),Y";
  return 2;
}

// absolute y
int doOpAby(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int arg = getWord(data + 1);
  ofs << op.name << " ";
  ofs.wordSplit(arg);
  ofs << ",Y";
  return 3;
}

// zero-page y
int doOpZpy(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int arg = getByte(data + 1);
  ofs << op.name;
  ofs.wordSplit(arg + hucZpOffset);
  ofs << ",Y";
  return 2;
}

// immediate
int doOpIm(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int arg = getByte(data + 1);
  ofs << op.name << " #";
  ofs.byte(arg);
  return 2;
}

// zero-page
int doOpZp(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int arg = getByte(data + 1);
  ofs << op.name << " ";
  ofs.wordSplit(arg + hucZpOffset);
  return 2;
}

// absolute
int doOpAb(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int arg = getWord(data + 1);
  ofs << op.name << " ";
  ofs.wordSplit(arg);
  return 3;
}

void printBranch(int dist, int offset, AsmOstream& ofs) {
  ofs.word(offset + dist);
  ofs << " [";
  if (dist > 0) {
    ofs << "+";
  }
  else {
    ofs << "-";
    dist = -dist;
  }
  ofs.byte(dist);
  ofs << "]";
}

// relative
int doOpRel(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int dist = getByte(data + 1, SignednessTypes::sign);
  ofs << op.name << " ";
  printBranch(dist, offset + 2, ofs);
  return 2;
}

// zero-page relative
int doOpZprel(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int addr = getByte(data + 1);
  int dist = getByte(data + 2, SignednessTypes::sign);
  ofs << op.name << " ";
  ofs.wordSplit(addr + hucZpOffset);
  ofs << ",";
  printBranch(dist, offset + 3, ofs);
  return 3;
}

// absolute indirect
int doOpAbind(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int addr = getAddress(data + 1);
  ofs << op.name << " (";
  ofs.wordSplit(addr);
  ofs << ")";
  return 3;
}

// absolute indirect x
int doOpAbindx(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int addr = getAddress(data + 1);
  ofs << op.name << " (";
  ofs.wordSplit(addr);
  ofs << ",X)";
  return 3;
}

// block
int doOpBlk(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int src = getAddress(data + 1);
  int dst = getAddress(data + 3);
  int len = getAddress(data + 5);
  ofs << op.name << " ";
  ofs.wordSplit(src);
  ofs << ",";
  ofs.wordSplit(dst);
  ofs << ",";
  ofs.word(len);
  return 7;
}

// immediate zero-page
int doOpImzp(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int bit = getByte(data + 1);
  int src = getByte(data + 2);
  ofs << op.name << " #";
  ofs.byte(bit);
  ofs << ",";
  ofs.wordSplit(src + hucZpOffset);
  return 3;
}

// immediate absolute
int doOpImab(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int bit = getByte(data + 1);
  int addr = getAddress(data + 2);
  ofs << op.name << " #";
  ofs.byte(bit);
  ofs << ",";
  ofs.wordSplit(addr);
  return 4;
}

// immediate zero-page x
int doOpImzpx(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int bit = getByte(data + 1);
  int src = getByte(data + 2);
  ofs << op.name << " #";
  ofs.byte(bit);
  ofs << ",";
  ofs.wordSplit(src + hucZpOffset);
  ofs << ",X";
  return 3;
}

// immediate absolute x
int doOpImabx(HucOp op, char* data, int offset, int size, AsmOstream& ofs) {
  int bit = getByte(data + 1);
  int addr = getAddress(data + 2);
  ofs << op.name << " #";
  ofs.byte(bit);
  ofs << ",";
  ofs.wordSplit(addr);
  ofs << ",X";
  return 4;
}

const HucOp opTable[] = {
  { 0x00, 1, "brk ", doOpGeneric },
  { 0x01, 2, "ora ", doOpIx },
  { 0x02, 1, "sxy ", doOpGeneric },
  { 0x03, 2, "st0 ", doOpIm },
  { 0x04, 2, "tsb ", doOpZp },
  { 0x05, 2, "ora ", doOpZp },
  { 0x06, 2, "asl ", doOpZp },
  { 0x07, 2, "rmb0", doOpZp },
  { 0x08, 1, "php ", doOpGeneric },
  { 0x09, 2, "ora ", doOpIm },
  { 0x0A, 1, "asl ", doOpGeneric },
  { 0x0B, 1, "", doOpInvalid },
  { 0x0C, 3, "tsb ", doOpAb },
  { 0x0D, 3, "ora ", doOpAb },
  { 0x0E, 3, "asl ", doOpAb },
  { 0x0F, 3, "bbr0", doOpZprel },
  
  { 0x10, 2, "bpl ", doOpRel },
  { 0x11, 2, "ora ", doOpIy },
  { 0x12, 3, "ora ", doOpZpind },
  { 0x13, 2, "st1 ", doOpIm },
  { 0x14, 2, "trb ", doOpZp },
  { 0x15, 2, "ora ", doOpZpx },
  { 0x16, 2, "asl ", doOpZpx },
  { 0x17, 2, "rmb1", doOpZp },
  { 0x18, 1, "clc ", doOpGeneric },
  { 0x19, 3, "ora ", doOpAby },
  { 0x1A, 1, "inc ", doOpGeneric },
  { 0x1B, 1, "", doOpInvalid },
  { 0x1C, 3, "trb ", doOpAb },
  { 0x1D, 3, "ora ", doOpAbx },
  { 0x1E, 3, "asl ", doOpAbx },
  { 0x1F, 3, "bbr1", doOpZprel },
  
  { 0x20, 3, "jsr ", doOpAb },
  { 0x21, 2, "and ", doOpIx },
  { 0x22, 1, "sax ", doOpGeneric },
  { 0x23, 2, "st2 ", doOpIm },
  { 0x24, 2, "bit ", doOpZp },
  { 0x25, 2, "and ", doOpZp },
  { 0x26, 2, "rol ", doOpZp },
  { 0x27, 2, "rmb2", doOpZp },
  { 0x28, 1, "plp ", doOpGeneric },
  { 0x29, 2, "and ", doOpIm },
  { 0x2A, 1, "rol ", doOpGeneric },
  { 0x2B, 1, "", doOpInvalid },
  { 0x2C, 3, "bit ", doOpAb },
  { 0x2D, 3, "and ", doOpAb },
  { 0x2E, 3, "rol ", doOpAb },
  { 0x2F, 3, "bbr2", doOpZprel },
  
  { 0x30, 2, "bmi ", doOpRel },
  { 0x31, 2, "and ", doOpIy },
  { 0x32, 3, "and ", doOpZpind },
  { 0x33, 1, "", doOpInvalid },
  { 0x34, 2, "bit ", doOpZpx },
  { 0x35, 2, "and ", doOpZpx },
  { 0x36, 2, "rol ", doOpZpx },
  { 0x37, 2, "rmb3", doOpZp },
  { 0x38, 1, "sec ", doOpGeneric },
  { 0x39, 3, "and ", doOpAby },
  { 0x3A, 1, "dec ", doOpGeneric },
  { 0x3B, 1, "", doOpInvalid },
  { 0x3C, 3, "bit ", doOpAbx },
  { 0x3D, 3, "and ", doOpAbx },
  { 0x3E, 3, "rol ", doOpAbx },
  { 0x3F, 3, "bbr3", doOpZprel },
  
  { 0x40, 1, "rti ", doOpGeneric },
  { 0x41, 2, "eor ", doOpIx },
  { 0x42, 1, "say ", doOpGeneric },
  { 0x43, 2, "tma ", doOpIm },
  { 0x44, 2, "bsr ", doOpRel },
  { 0x45, 2, "eor ", doOpZp },
  { 0x46, 2, "lsr ", doOpZp },
  { 0x47, 2, "rmb4", doOpZp },
  { 0x48, 1, "pha ", doOpGeneric },
  { 0x49, 2, "eor ", doOpIm },
  { 0x4A, 1, "lsr ", doOpGeneric },
  { 0x4B, 1, "", doOpInvalid },
  { 0x4C, 3, "jmp ", doOpAb },
  { 0x4D, 3, "eor ", doOpAb },
  { 0x4E, 3, "lsr ", doOpAb },
  { 0x4F, 3, "bbr4", doOpZprel },
  
  { 0x50, 2, "bvc ", doOpRel },
  { 0x51, 2, "eor ", doOpIy },
  { 0x52, 3, "eor ", doOpZpind },
  { 0x53, 2, "tam ", doOpIm },
  { 0x54, 1, "csl ", doOpGeneric },
  { 0x55, 2, "eor ", doOpZpx },
  { 0x56, 2, "lsr ", doOpZpx },
  { 0x57, 2, "rmb5", doOpZp },
  { 0x58, 1, "cli ", doOpGeneric },
  { 0x59, 3, "eor ", doOpAby },
  { 0x5A, 1, "phy ", doOpGeneric },
  { 0x5B, 1, "", doOpInvalid },
  { 0x5C, 1, "", doOpInvalid },
  { 0x5D, 3, "eor ", doOpAbx },
  { 0x5E, 3, "lsr ", doOpAbx },
  { 0x5F, 3, "bbr5", doOpZprel },
  
  { 0x60, 1, "rts ", doOpGeneric },
  { 0x61, 2, "adc ", doOpIx },
  { 0x62, 1, "cla ", doOpGeneric },
  { 0x63, 1, "", doOpInvalid },
  { 0x64, 2, "stz ", doOpZp },
  { 0x65, 2, "adc ", doOpZp },
  { 0x66, 2, "ror ", doOpZp },
  { 0x67, 2, "rmb6", doOpZp },
  { 0x68, 1, "pla ", doOpGeneric },
  { 0x69, 2, "adc ", doOpIm },
  { 0x6A, 1, "ror ", doOpGeneric },
  { 0x6B, 1, "", doOpInvalid },
  { 0x6C, 3, "jmp ", doOpAbind },
  { 0x6D, 3, "adc ", doOpAb },
  { 0x6E, 3, "ror ", doOpAb },
  { 0x6F, 3, "bbr6", doOpZprel },
  
  { 0x70, 2, "bvs ", doOpRel },
  { 0x71, 2, "adc ", doOpIy },
  { 0x72, 3, "adc ", doOpZpind },
  { 0x73, 7, "tii ", doOpBlk },
  { 0x74, 2, "stz ", doOpZpx },
  { 0x75, 2, "adc ", doOpZpx },
  { 0x76, 2, "ror ", doOpZpx },
  { 0x77, 2, "rmb7", doOpZp },
  { 0x78, 1, "sei ", doOpGeneric },
  { 0x79, 3, "adc ", doOpAby },
  { 0x7A, 1, "ply ", doOpGeneric },
  { 0x7B, 1, "", doOpInvalid },
  { 0x7C, 3, "jmp ", doOpAbindx },
  { 0x7D, 3, "adc ", doOpAbx },
  { 0x7E, 3, "ror ", doOpAbx },
  { 0x7F, 3, "bbr7", doOpZprel },
  
  { 0x80, 2, "bra ", doOpRel },
  { 0x81, 2, "sta ", doOpIx },
  { 0x82, 1, "clx ", doOpGeneric },
  { 0x83, 3, "tst ", doOpImzp },
  { 0x84, 2, "sty ", doOpZp },
  { 0x85, 2, "sta ", doOpZp },
  { 0x86, 2, "stx ", doOpZp },
  { 0x87, 2, "smb0", doOpZp },
  { 0x88, 1, "dey ", doOpGeneric },
  { 0x89, 2, "bit ", doOpIm },
  { 0x8A, 1, "txa ", doOpGeneric },
  { 0x8B, 1, "", doOpInvalid },
  { 0x8C, 3, "sty ", doOpAb },
  { 0x8D, 3, "sta ", doOpAb },
  { 0x8E, 3, "stx ", doOpAb },
  { 0x8F, 3, "bbs0", doOpZprel },
  
  { 0x90, 2, "bcc ", doOpRel },
  { 0x91, 2, "sta ", doOpIy },
  { 0x92, 3, "sta ", doOpZpind },
  { 0x93, 4, "tst ", doOpImab },
  { 0x94, 2, "sty ", doOpZpx },
  { 0x95, 2, "sta ", doOpZpx },
  { 0x96, 2, "stx ", doOpZpy },
  { 0x97, 2, "smb1", doOpZp },
  { 0x98, 1, "tya ", doOpGeneric },
  { 0x99, 3, "sta ", doOpAby },
  { 0x9A, 1, "txs ", doOpGeneric },
  { 0x9B, 1, "", doOpInvalid },
  { 0x9C, 3, "stz ", doOpAb },
  { 0x9D, 3, "sta ", doOpAbx },
  { 0x9E, 3, "stz ", doOpAbx },
  { 0x9F, 3, "bbs1", doOpZprel },
  
  { 0xA0, 2, "ldy ", doOpIm },
  { 0xA1, 2, "lda ", doOpIx },
  { 0xA2, 2, "ldx ", doOpIm },
  { 0xA3, 3, "tst ", doOpImzpx },
  { 0xA4, 2, "ldy ", doOpZp },
  { 0xA5, 2, "lda ", doOpZp },
  { 0xA6, 2, "ldx ", doOpZp },
  { 0xA7, 2, "smb2", doOpZp },
  { 0xA8, 1, "tay ", doOpGeneric },
  { 0xA9, 2, "lda ", doOpIm },
  { 0xAA, 1, "tax ", doOpGeneric },
  { 0xAB, 1, "", doOpInvalid },
  { 0xAC, 3, "ldy ", doOpAb },
  { 0xAD, 3, "lda ", doOpAb },
  { 0xAE, 3, "ldx ", doOpAb },
  { 0xAF, 3, "bbs2", doOpZprel },
  
  { 0xB0, 2, "bcs ", doOpRel },
  { 0xB1, 2, "lda ", doOpIy },
  { 0xB2, 3, "lda ", doOpZpind },
  { 0xB3, 4, "tst ", doOpImabx },
  { 0xB4, 2, "ldy ", doOpZpx },
  { 0xB5, 2, "lda ", doOpZpx },
  { 0xB6, 2, "ldx ", doOpZpy },
  { 0xB7, 2, "smb3", doOpZp },
  { 0xB8, 1, "clv ", doOpGeneric },
  { 0xB9, 3, "lda ", doOpAby },
  { 0xBA, 1, "tsx ", doOpGeneric },
  { 0xBB, 1, "", doOpInvalid },
  { 0xBC, 3, "ldy ", doOpAbx },
  { 0xBD, 3, "lda ", doOpAbx },
  { 0xBE, 3, "ldx ", doOpAby },
  { 0xBF, 3, "bbs3", doOpZprel },
  
  { 0xC0, 2, "cpy ", doOpIm },
  { 0xC1, 2, "cmp ", doOpIx },
  { 0xC2, 1, "cly ", doOpGeneric },
  { 0xC3, 7, "tdd ", doOpBlk },
  { 0xC4, 2, "cpy ", doOpZp },
  { 0xC5, 2, "cmp ", doOpZp },
  { 0xC6, 2, "dec ", doOpZp },
  { 0xC7, 2, "smb4", doOpZp },
  { 0xC8, 1, "iny ", doOpGeneric },
  { 0xC9, 2, "cmp ", doOpIm },
  { 0xCA, 1, "dex ", doOpGeneric },
  { 0xCB, 1, "", doOpInvalid },
  { 0xCC, 3, "cpy ", doOpAb },
  { 0xCD, 3, "cmp ", doOpAb },
  { 0xCE, 3, "dec ", doOpAb },
  { 0xCF, 3, "bbs4", doOpZprel },
  
  { 0xD0, 2, "bne ", doOpRel },
  { 0xD1, 2, "cmp ", doOpIy },
  { 0xD2, 3, "cmp ", doOpZpind },
  { 0xD3, 7, "tin ", doOpBlk },
  { 0xD4, 1, "csh ", doOpGeneric },
  { 0xD5, 2, "cmp ", doOpZpx },
  { 0xD6, 2, "dec ", doOpZpx },
  { 0xD7, 2, "smb5", doOpZp },
  { 0xD8, 1, "cld ", doOpGeneric },
  { 0xD9, 3, "cmp ", doOpAby },
  { 0xDA, 1, "phx ", doOpGeneric },
  { 0xDB, 1, "", doOpInvalid },
  { 0xDC, 1, "", doOpInvalid },
  { 0xDD, 3, "cmp ", doOpAbx },
  { 0xDE, 3, "dec ", doOpAbx },
  { 0xDF, 3, "bbs5", doOpZprel },
  
  { 0xE0, 2, "cpx ", doOpIm },
  { 0xE1, 2, "sbc ", doOpIx },
  { 0xE2, 1, "", doOpInvalid },
  { 0xE3, 7, "tia ", doOpBlk },
  { 0xE4, 2, "cpx ", doOpZp },
  { 0xE5, 2, "sbc ", doOpZp },
  { 0xE6, 2, "inc ", doOpZp },
  { 0xE7, 2, "smb6", doOpZp },
  { 0xE8, 1, "inx ", doOpGeneric },
  { 0xE9, 2, "sbc ", doOpIm },
  { 0xEA, 1, "nop ", doOpGeneric },
  { 0xEB, 1, "", doOpInvalid },
  { 0xEC, 3, "cpx ", doOpAb },
  { 0xED, 3, "sbc ", doOpAb },
  { 0xEE, 3, "inc ", doOpAb },
  { 0xEF, 3, "bbs6", doOpZprel },
  
  { 0xF0, 2, "beq ", doOpRel },
  { 0xF1, 2, "sbc ", doOpIy },
  { 0xF2, 3, "sbc ", doOpZpind },
  { 0xF3, 7, "tai ", doOpBlk },
  { 0xF4, 1, "set ", doOpGeneric },
  { 0xF5, 2, "sbc ", doOpZpx },
  { 0xF6, 2, "inc ", doOpZpx },
  { 0xF7, 2, "smb7", doOpZp },
  { 0xF8, 1, "sed ", doOpGeneric },
  { 0xF9, 3, "sbc ", doOpAby },
  { 0xFA, 1, "plx ", doOpGeneric },
  { 0xFB, 1, "", doOpInvalid },
  { 0xFC, 1, "", doOpInvalid },
  { 0xFD, 3, "sbc ", doOpAbx },
  { 0xFE, 3, "inc ", doOpAbx },
  { 0xFF, 3, "bbs7", doOpZprel },
};

int doOp(char* data, int offset, int size, AsmOstream& ofs) {
  
  int code = *((unsigned char*)(data));
  
//  if (code > 0x01) {
//    return doOpInvalid({ 0, 1, "unk", NULL }, data, size, ofs);
//  }
  
  const HucOp& op = opTable[code];
  
  ofs.setBaseInd(false);
  int bsize = (op.size <= size) ? op.size : 1;
  int totalchars = 0;
  for (int i = 0; i < bsize; i++) {
    ofs << " ";
    ofs.byte((int)((unsigned char)(*(data + i))));
    totalchars += 3;
  }
  ofs.setBaseInd(true);
  
  for (int i = totalchars; i < instructionSpacing; i++) {
    ofs << ' ';
  }
  
  if (op.size > size) {
    return doOpInvalid(op, data, offset, size, ofs);
  }
  else {
    return (*(op.handler))(op, data, offset, size, ofs);
  }
}

void disassembleHuc(char* data, int size, AsmOstream& ofs) {
  int pos = 0;
  while (pos < size) {
    ofs << "$";
    ofs.internalStream() << hex << uppercase;
    ofs.setFill('0');
    ofs.setW(addressLength);
    ofs << pos << " ";
    pos += doOp(data + pos, pos, size - pos, ofs);
    ofs << '\n';
  }
}

int main(int argc, char* argv[]) {
  
  if ((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
    cout << "hucdism: Naive HuC6280 disassembler" << endl;
    cout << "Usage: hucdism [options] <file>" << endl;
    cout << "Options:" << endl;
    cout << "\t-b\tSpecifies starting offset within file" << endl;
    cout << "\t-s\tSpecifies number of bytes to disassemble" << endl;
    cout << "\t-c\tPrints disassembly in upper case" << endl;
    cout << "\t-a\tEnables paged address display" << endl;
    cout << "\t-mpr*\tIf -a if set, addresses targeting the given MPR will be"
           << endl
           << "\t\tdecoded to their corresponding physical address."
           << endl
           << "\t\tFor example, '-mpr1 0xF8' will decode $2xxx to $1F0xxx."
           << endl;
    return 0;
  }
  
  AsmOstream ofs(cout);
  int startingOffset = -1;
  int size = -1;
  for (int i = 1; i < argc - 1; i++) {
    if (strcmp(argv[i], "-b") == 0) {
      startingOffset
        = TStringConversion::stringToInt(string(argv[i + 1]));
    }
    else if (strcmp(argv[i], "-s") == 0) {
      size
        = TStringConversion::stringToInt(string(argv[i + 1]));
    }
    else if (strcmp(argv[i], "-c") == 0) {
      ofs.setCaps(true);
    }
    else if (strcmp(argv[i], "-a") == 0) {
      useSplitAddresses = true;
    }
    else if ((strlen(argv[i]) >= 5)
             && (memcmp(argv[i], "-mpr", 4) == 0)) {
      int reg = TStringConversion::stringToInt(
                  string() + argv[i][4]);
      if (reg <= 7) {
        knownMPRs[reg]
          = TStringConversion::stringToInt(string(argv[i + 1]));
      }
    }
  }
  
  ifstream ifs(argv[argc - 1], ios_base::binary);
  int filesize = fsize(ifs);
  
  if (startingOffset < 0) {
    startingOffset = 0;
  }
  
  if ((size < 0)
      || ((startingOffset + size) > filesize)) {
    size = filesize - startingOffset;
  }
  
  char* infile = new char[filesize];
  ifs.read(infile, filesize);
  disassembleHuc(infile + startingOffset, size, ofs);
  delete infile;
  
  return 0;
}
