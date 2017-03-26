#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TStringConversion.h"
#include "util/ByteConversion.h"
#include "util/TTwoDArray.h"
#include "util/TByte.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;
using namespace BlackT;

int fsize(std::istream& ifs) {
  int pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(pos);
  return sz;
}

void doStatSwap(TByte* output,
                TByte* script, TByte* script2,
                int scriptSize, int script2Size) {
  int limit = (scriptSize < script2Size)
                ? scriptSize
                : script2Size;
  limit -= 6;
  
//  cout << limit << endl;
  
  int i = 0;
  while (i < limit) {
    if ((script[i] == 0x6A) && (script2[i] == 0x6A)) {
      bool ok = false;
      
/*      cout << hex << (int)(script[i]) << " "
                  << (int)(script[i + 1]) << " "
                  << (int)(script[i + 2]) << " "
                  << (int)(script[i + 3]) << " "
                  << (int)(script2[i]) << " "
                  << (int)(script2[i + 1]) << " "
                  << (int)(script2[i + 2]) << " "
                  << (int)(script2[i + 3]) << endl; */
//      if (((script[i + 1])
//                == (script2[i + 1]))) {
//        cerr << i << endl;
//      }
      if ((script[i + 1] == 0x13)
          && (script2[i + 1] == 0x13)) {
        ok = true;
      }
      else if ((script[i + 1] == 0x12)
          && (script2[i + 1] == 0x12)) {
        ok = true;
      }
      
      if ((script[i + 2] != script2[i + 2])
          || (script[i + 3] != script2[i + 3])) {
        ok = false;
      }
      
      int as16 = ByteConversion::fromBytes(script + i + 2, 2,
                                            EndiannessTypes::big,
                                            SignednessTypes::nosign);
      
      if ((as16 < 0xD0) || (as16 > 0x1D0)) {
        ok = false;
      }
      
/*      if (((script[i + 1]) == (script2[i + 1]))
            && (script[i + 2] == script2[i + 2])
            && (script[i + 3] == script2[i + 3])) {
        ok = true;
      } */
      
      if (!ok) {
        ++i;
        continue;
      }
//        cout << "HERE" << endl;
      
      // skip unknown
      i += 4;
      
      int old1 = script[i];
      int old2 = script[i + 1];
      int new1 = script2[i];
      int new2 = script2[i + 1];
      
      if ((old1 != new1) || (old2 != new2)) {
        int first = ByteConversion::fromBytes(script + i, 2,
                                              EndiannessTypes::big,
                                              SignednessTypes::nosign);
        int second = ByteConversion::fromBytes(script2 + i, 2,
                                              EndiannessTypes::big,
                                              SignednessTypes::nosign);
      
/*        cout << "i: " << hex << i << endl;
        cout << "command: " << (int)(script[i - 4]) << " "
                            << (int)(script[i - 3]) << endl;
        cout << "as16: " << as16 << endl;
        cout << "old: " << (int)(script[i])
          << " " << (int)(script[i + 1]) << endl;
        cout << "new: " << (int)(script2[i])
          << " " << (int)(script2[i + 1]) << endl; */
        
        if (first < second) {
          // old: skip
//          cout << "ERROR: new greater than old, skipping!" << endl;
//          cout << endl;
//          continue;

          // new -- use this
/*          cout << "WARNING: New greater than old" << endl;
          cout << endl; */
        }
        else if (as16 >= 0xF0) {
          // old: skip
//          cout << "ERROR: high as16, skipping!" << endl;
//          cout << endl;
//          continue;

          // new -- use this
/*          cout << "WARNING: High as16 -- may be bad" << endl;
          cout << endl; */
        }
        else {
/*          cout << endl; */
        }
        
      }
      else {
        continue;
      }
      
      
      // copy value
      output[i] = script2[i];
      output[i + 1] = script2[i + 1];
      
      i += 2;
    }
    else {
      ++i;
    }
  }
}

void statSwap(TByte* output, TByte* input, TByte* input2, int numEntries,
              int chunkEnd, int chunkEnd2) {
  // find end of LUTs
/*  int i1_end = ByteConversion::fromBytes(
    input,
    ByteSizes::uint32Size,
    EndiannessTypes::big,
    SignednessTypes::nosign);
  int i2_end = ByteConversion::fromBytes(
    input2,
    ByteSizes::uint32Size,
    EndiannessTypes::big,
    SignednessTypes::nosign);
  
//  int i1_size = i1_end / 4;
//  int i2_size = i2_end / 4;
//  int i1_size = 0x1E;
//  int i2_size = 0x1E;
  
  if (i1_size != i2_size) {
    cerr << i1_size << " " << i2_size << endl;
  } */
  
  for (int i = 0; i < numEntries; i++) {
    unsigned int i1_addr = ByteConversion::fromBytes(
      input + (i * ByteSizes::uint32Size),
      ByteSizes::uint32Size,
      EndiannessTypes::big,
      SignednessTypes::nosign);
    unsigned int i2_addr = ByteConversion::fromBytes(
      input2 + (i * ByteSizes::uint32Size),
      ByteSizes::uint32Size,
      EndiannessTypes::big,
      SignednessTypes::nosign);
      
    if ((i1_addr == 0xFFFFFFFF)
        || (i2_addr == 0xFFFFFFFF)) {
      continue;
    }
    
    int i1_size, i2_size;
    if (i == numEntries - 1) {
      i1_size = chunkEnd - i1_addr;
      i2_size = chunkEnd2 - i2_addr;
    }
    else {
      unsigned int i1_next = ByteConversion::fromBytes(
        input + ((i + 1) * ByteSizes::uint32Size),
        ByteSizes::uint32Size,
        EndiannessTypes::big,
        SignednessTypes::nosign);
      unsigned int i2_next = ByteConversion::fromBytes(
        input2 + ((i + 1) * ByteSizes::uint32Size),
        ByteSizes::uint32Size,
        EndiannessTypes::big,
        SignednessTypes::nosign);
      
      i1_size = i1_next - i1_addr;
      i2_size = i2_next - i2_addr;
    }
    
//    cout << i1_addr << " " << i2_addr << endl;
    
    doStatSwap(output + i1_addr,
               input + i1_addr, input2 + i2_addr,
               i1_size, i2_size);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    return 0;
  }
  
  ifstream ifs(argv[1], ios_base::binary);
  ifstream ifs2(argv[2], ios_base::binary);
  ofstream ofs(argv[3], ios_base::binary);
  
  int sz = fsize(ifs);
  int sz2 = fsize(ifs2);
  
  TByte* input = new TByte[sz];
  TByte* input2 = new TByte[sz2];
  TByte* output = new TByte[sz];
  ifs.read((char*)input, sz);
  ifs2.read((char*)input2, sz2);
  
  std::memcpy(output, input, sz);
  
  int blockstart = ByteConversion::fromBytes(
    input + 4,
    ByteSizes::uint32Size,
    EndiannessTypes::big,
    SignednessTypes::nosign);
  int blockstart2 = ByteConversion::fromBytes(
    input2 + 4,
    ByteSizes::uint32Size,
    EndiannessTypes::big,
    SignednessTypes::nosign);
  
  int blockend = ByteConversion::fromBytes(
    input + 8,
    ByteSizes::uint32Size,
    EndiannessTypes::big,
    SignednessTypes::nosign);
  int blockend2 = ByteConversion::fromBytes(
    input2 + 8,
    ByteSizes::uint32Size,
    EndiannessTypes::big,
    SignednessTypes::nosign);
    
  if (blockend != blockend2) {
//    cerr << "WARNING: Block size mismatch!" << endl;
//    cerr << "end1: " << blockend << ", end2: " << blockend2 << endl;
  }
  
//  int blocksize = blockend2 - blockstart2;
  
  // take the smaller block size
//  if (blockend2 < blockend) {
//    blocksize = blockend2 - blockstart;
//  }

//  std::memcpy(output + blockstart, input2 + blockstart, blocksize);
  
  // copy index
//  ofs.write((char*)(input), 16);
  // copy block 0 from input 2
//  ofs.write((char*)(input2 + 16), (blockstart - 16));

//  ofs.write("\x00\x00\x00\x10", 4);
  // index placeholders
//  ofs.write("\x00\x00\x00\x00", 4);
//  ofs.write("\x00\x00\x00\x00", 4);
//  ofs.write("\x00\x00\x00\x00", 4);

  // do stat replacements in block 0
  statSwap(output + 0x10, input + 0x10, input2 + 0x10,
            0x44 / 4, blockstart, blockstart2);
  // do stat replacements in block 1
  statSwap(output + blockstart, input + blockstart, input2 + blockstart2,
            0x78 / 4, blockend, blockend2);
    
  ofs.write((char*)output, sz);
    
  delete input;
  delete input2;
  delete output;
  
  return 0;
  
/*  if (argc < 4) {
    return 0;
  }
  
  ifstream ifs(argv[1], ios_base::binary);
  ifstream ifs2(argv[2], ios_base::binary);
  ofstream ofs(argv[3], ios_base::binary);
  
  int sz = fsize(ifs);
  int sz2 = fsize(ifs2);
  
  TByte* input = new TByte[sz];
  TByte* input2 = new TByte[sz2];
  TByte* output = new TByte[sz];
  ifs.read((char*)input, sz);
  ifs2.read((char*)input2, sz2);
  
  std::memcpy(output, input, sz);
  
  int blockstart = ByteConversion::fromBytes(
    input + 4,
    ByteSizes::uint32Size,
    EndiannessTypes::big,
    SignednessTypes::nosign);
  
  int blockend = ByteConversion::fromBytes(
    input + 8,
    ByteSizes::uint32Size,
    EndiannessTypes::big,
    SignednessTypes::nosign);
  int blockend2 = ByteConversion::fromBytes(
    input2 + 8,
    ByteSizes::uint32Size,
    EndiannessTypes::big,
    SignednessTypes::nosign);
    
  if (blockend != blockend2) {
    cerr << "WARNING: Block size mismatch!" << endl;
    cerr << "end1: " << blockend << ", end2: " << blockend2 << endl;
  }
  
  int blocksize = blockend2 - blockstart;
  
  // take the smaller block size
//  if (blockend2 < blockend) {
//    blocksize = blockend2 - blockstart;
//  }
  
  std::memcpy(output + blockstart, input2 + blockstart, blocksize);
  
  // copy index
  ofs.write((char*)(input2), 12);
  // filesize (placeholder: fill in after finishing everything else)
  ofs.write("\x00\x00\x00\x00", 4);
  // copy block 0 from input 1
  ofs.write((char*)(input2 + 16), (blockstart - 16));
  // write block 1 from input 2
  ofs.write((char*)(input2 + blockstart), blocksize);
  // write block 2 from input 1
  ofs.write((char*)(input + blockend), sz - blockend);
  
  // write total size
  int totalsize = ofs.tellp();
  ofs.seekp(12);
  TByte buffer[4];
  ByteConversion::toBytes(
    totalsize, buffer, 4, EndiannessTypes::big, SignednessTypes::nosign);
  ofs.write((char*)buffer, 4);
    
//  ofs.write((char*)output, sz);
    
  delete input;
  delete input2;
  delete output;
  
  return 0; */
}
