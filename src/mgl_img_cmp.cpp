#include "mgl_cmpr.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

// maximum size of compressed data -- don't exceed this
const static int outputBufferSize = 0x100000;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Mahou Gakuen Lunar! image data compressor" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
//    cout << "Options: " << endl;
//    cout << "  -s [startpos]   Specifies starting offset in infile.\n";
//    cout << "  -l [length]     Specifies size of data in infile.\n";
    return 0;
  }
  
  int startpos = 0;
  int length = -1;
  
/*  for (int i = 3; i < argc - 1; i++) {
    if (strcmp(argv[i], "-s") == 0) {
      sscanf(argv[i + 1], "%i", &startpos);
    }
    else if (strcmp(argv[i], "-l") == 0) {
      sscanf(argv[i + 1], "%i", &length);
    }
  } */
  
  ifstream ifs(argv[1], ios_base::binary);
  
  // if no length specified, default to size of input file
  if (length == -1) {
    ifs.seekg(0, ios_base::end);
    length = ifs.tellg();
  }
  
  // prepare output buffer
  unsigned char outputBuf[outputBufferSize];
  
  // read input data to buffer
  ifs.seekg(startpos);
  unsigned char* input = new unsigned char[length];
  ifs.read((char*)input, length);
  
  int outputSize = compressMglImg(input, length, outputBuf);
  
  // clean up
  delete input;
  
  // write compressed data to file
  ofstream ofs(argv[2], ios_base::binary);
  ofs.write((char*)outputBuf, outputSize);
  
  return 0;
}
