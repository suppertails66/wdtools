#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

// maximum size of decompressed data -- don't exceed this
const static int outputBufferSize = 0x100000;

const static int decompressionBufferSize = 0x1000;
const static int decmpBufferWrap = 0xFFF;
const static int decmpBufferStartPos = 0xFEE;

// Decompresses image data, returning size of the decompressed data
int decompressMglImg(const unsigned char* input,
                     int length,
                     unsigned char* outputBuf) {
  // prepare decompression buffer
  unsigned char decmpBuf[decompressionBufferSize];
  std::memset(decmpBuf, 0, decompressionBufferSize);
  
  int inputPos = 0;
  int decmpBufPos = decmpBufferStartPos;
  int outputBufPos = 0;
  int nextCommandByteCounter = 0;
  int commandByte = 0;
  
  // decompress the data
  while (inputPos < length) {
    // have we consumed all the bits in the command byte?
    // (the use of a bitshift counter instead of a for loop here is to more
    // closely match the structure of the original function)
    bool commandNeeded = !(nextCommandByteCounter & 0x01);
    nextCommandByteCounter >>= 1;
    
    // if so, read a new command byte
    if (commandNeeded) {
      commandByte = input[inputPos++];
      
      // break if done
      if (inputPos >= length) break;
      
      // reset counter to next command
      nextCommandByteCounter = 0x7F;
    }
    
    // get next command bit
    int command = commandByte & 0x01;
    commandByte >>= 1;
    
    // get next byte
    unsigned char byte1 = input[inputPos++];
    
    // command bit set: absolute
    if ((bool)command) {
      // write to output and decompression buffers
      outputBuf[outputBufPos++] = byte1;
      decmpBuf[decmpBufPos++] = byte1;
      
      // wrap decompression buffer
      decmpBufPos &= decmpBufferWrap;

      // break if done
      if (inputPos >= length) break;
    }
    // command bit unset: relative
    else {
      // get next byte
      unsigned char byte2 = input[inputPos++];
      
      // break if done
      if (inputPos >= length) break;
      
      // the high nybble of byte2 combines with byte1 to form a 12-bit absolute
      // offset into the decompression buffer
      int offset = ((int)(byte2 & 0xF0) << 4) | byte1;
      
      // the low nybble of byte2 is the loop count, minus 3
      int numLoops = (byte2 & 0x0F) + 3;
      
      // copy characters from decompression buffer
      for (int i = 0; i < numLoops; i++) {
        // get next char (wrapping around buffer if necessary)
        unsigned char next = decmpBuf[(offset + i) & decmpBufferWrap];
        
        // write to output and decompression buffers
        outputBuf[outputBufPos++] = next;
        decmpBuf[decmpBufPos++] = next;
        decmpBufPos &= decmpBufferWrap;
      }
    }
  }
  
  // return size of decompressed data
  return outputBufPos;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Mahou Gakuen Lunar! image data decompressor" << endl;
    cout << "Usage: " << argv[0] << "<infile> <outfile> [options]" << endl;
    cout << "Options: " << endl;
    cout << "  -s [startpos]   Specifies starting offset in infile.\n";
    cout << "  -l [length]     Specifies size of data in infile.\n";
    return 0;
  }
  
  int startpos = 0;
  int length = -1;
  
  for (int i = 3; i < argc - 1; i++) {
    if (strcmp(argv[i], "-s") == 0) {
      sscanf(argv[i + 1], "%i", &startpos);
    }
    else if (strcmp(argv[i], "-l") == 0) {
      sscanf(argv[i + 1], "%i", &length);
    }
  }
  
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
  
  int outputSize = decompressMglImg(input, length, outputBuf);
  
  // clean up
  delete input;
  
  // write decompressed data to file
  ofstream ofs(argv[2], ios_base::binary);
  ofs.write((char*)outputBuf, outputSize);
  
  return 0;
}
