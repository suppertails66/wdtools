#include "mgl_cmpr.h"
#include "util/ByteConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TStringConversion.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <map>

using namespace std;
using namespace BlackT;

// Size of the buffer used to concatenate the VRAM data
const int vramCatBufferSize = 0x100000;

// Size of the buffer used to compress the VRAM data
const int vramCompressionBufferSize = 0x100000;

int main(int argc, char* argv[]) {
  
  // mgl_img_insr indexfile indexoffset outindex imagefile imageoffset outimage inpath
  if (argc < 8) {
    
    return 0;
  }
  
  string indexFileName(argv[1]);
  int indexOffset = TStringConversion::stringToInt(string(argv[2]));
  string indexOutputFileName(argv[3]);
  string imageFileName(argv[4]);
  int imageOffset = TStringConversion::stringToInt(string(argv[5]));
  string imageOutputFileName(argv[6]);
  string inPath(argv[7]);
  
  // Buffer to hold combined image and palette data
  char vramCatBuffer[vramCatBufferSize];
  int vramCatBufferPos = 0;
  std::memset(vramCatBuffer, 0, vramCatBufferSize);
  
  // Maps of file names to their offsets in the VRAM buffer
  map<string, int> imageNameToOffset;
  map<string, int> paletteNameToOffset;
  
  // Load the glue file
  vector<GlueFileEntry> glueFileEntries;
  ifstream glueIfs(inPath + glueFileName);
  while (glueIfs.good()) {
    GlueFileEntry entry;
    entry.load(glueIfs);
    if (!glueIfs.good()) break;
//    cout << entry.num << endl;
    glueFileEntries.push_back(entry);
  }
  
  // Open each sequential image file, convert to 4bpp, and place in the buffer
  for (int i = 0; ; i++) {
    string baseFilename = makeImageFileName(i);
    string filename = inPath + baseFilename;
    if (!fileExists(filename.c_str())) break;
    
    TGraphic g;
    TPngConversion::RGBAPngToGraphic(filename, g);
    
    // Record image data offset
    imageNameToOffset[baseFilename] = vramCatBufferPos;
    
    // Convert to 4bpp
    vramCatBufferPos += writeGraphic4bpp(g,
      (unsigned char*)vramCatBuffer + vramCatBufferPos);
    
    // Maintain required byte alignment
    vramCatBufferPos = getVramAlignedOffset(vramCatBufferPos);
  }
  
  // Open each sequential palette and place in buffer
  for (int i = 0; ; i++) {
    string baseFilename = makePaletteFileName(i);
    string filename = inPath + baseFilename;
    if (!fileExists(filename.c_str())) break;
    
    // Record palette data offset
    paletteNameToOffset[baseFilename] = vramCatBufferPos;
    
    // Copy palette into buffer
    BufferedFile file = getFile(filename.c_str());
    std::memcpy(vramCatBuffer + vramCatBufferPos, file.buffer, file.size);
    vramCatBufferPos += file.size;
    delete file.buffer;
    
    // Maintain required byte alignment
//    vramCatBufferPos = getVramAlignedOffset(vramCatBufferPos);
  }
  
  // Add trailer, if it exists, to buffer
  if (fileExists(inPath + trailerFileName)) {
    BufferedFile file = getFile((inPath + trailerFileName).c_str());
    std::memcpy(vramCatBuffer + vramCatBufferPos, file.buffer, file.size);
    vramCatBufferPos += file.size;
    delete file.buffer;
  }
  
//  ofstream temp("concatenated.bin");
//  temp.write((char*)vramCatBuffer, vramCatBufferPos);
  
  // Compress the combined VRAM data
  char vramCompressionBuffer[vramCompressionBufferSize];
  int compressedDataSize
    = compressMglImg((unsigned char*)vramCatBuffer,
                   vramCatBufferPos,
                   (unsigned char*)vramCompressionBuffer);
  
  // Read the target image file
  BufferedFile imageFile = getFile((imageFileName).c_str());
  char* imageP = imageFile.buffer + imageOffset;
  int originalImageSize = ByteConversion::fromBytes(imageP + 0, 4,
    EndiannessTypes::big, SignednessTypes::nosign);
  
  // Make sure there's enough room for the new data
  if (compressedDataSize > originalImageSize) {
    cerr << "Error: compressed data too large!" << endl;
    cerr << "Original: " << originalImageSize << endl;
    cerr << "New: " << compressedDataSize << endl;
    return 1;
  }
  
  // Update the data and size
  std::memcpy(imageP + 4, vramCompressionBuffer, compressedDataSize);
  ByteConversion::toBytes(compressedDataSize, imageP + 0, 4,
    EndiannessTypes::big, SignednessTypes::nosign);
  
//  cout << originalImageSize << " " << compressedDataSize << endl;

  // Write to file
  saveFile(imageFile, imageOutputFileName.c_str());

  // Clean up
  delete imageFile.buffer;

  // Read the target index file
  BufferedFile indexFile = getFile((indexFileName).c_str());
  char* indexP = indexFile.buffer + indexOffset;
  
  unsigned int vramTablesPointer = ByteConversion::fromBytes(indexP + 4, 4,
    EndiannessTypes::big, SignednessTypes::nosign);
//  unsigned int numVramTableEntries = ByteConversion::fromBytes(indexP + 8, 4,
//    EndiannessTypes::big, SignednessTypes::nosign);
  
//  cout << vramTablesPointer << endl;
  
  // Update the index
  for (unsigned int i = 0; i < glueFileEntries.size(); i++) {
    GlueFileEntry& entry = glueFileEntries[i];
    
    // Update target character and palette offsets
    entry.vramTableEntry.setSourceOffset(
      imageNameToOffset[entry.imageFileName]);
    entry.vramTableEntry.setPaletteOffset(
      paletteNameToOffset[entry.paletteFileName]);
    
//    cout << entry.vramTableEntry.paletteOffset() << endl;
    
    // Write to index
    entry.vramTableEntry.toData(indexP + vramTablesPointer
      + (i * vramTableEntrySize));
  }

  // Write to file
  saveFile(indexFile, indexOutputFileName.c_str());

  // Clean up
  delete indexFile.buffer;
  
  return 0;
} 
