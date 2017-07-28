#include "mgl_cmpr.h"
#include "util/ByteConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TStringConversion.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace BlackT;

// Size of the buffer used to decompress the VRAM data
const int vramDecompressionBufferSize = 0x100000;

int main(int argc, char* argv[]) {
  // mgl_img_extr indexfile indexoffset imagefile imageoffset outpath
  if (argc < 6) {
    cout << "Mahou Gakuen Lunar! image data extractor" << endl;
    cout << "Usage: " << argv[0] << " <ifile> <ioffset>"
           << " <imgfile> <imgoffset> <outpath>" << endl << endl;
    cout << "  ifile      Index file containing VRAM command table list\n";
    cout << "  ioffset    Offset in the file of the aforementioned list\n";
    cout << "  imgfile    File containing image data to read\n";
    cout << "  imgoffset  Offset of the aforementioned image data\n";
    cout << "  outpath    Directory to which to output files\n";
    
    return 0;
  }
  
  string indexFileName(argv[1]);
  int indexOffset = TStringConversion::stringToInt(string(argv[2]));
  string imageFileName(argv[3]);
  int imageOffset = TStringConversion::stringToInt(string(argv[4]));
  string outPath(argv[5]);
  
  // Open the index file
  BufferedFile indexFile = getFile(indexFileName.c_str());
  
  // Get pointer to index data
  char* indexP = indexFile.buffer + indexOffset;
  
  // Read index header
  unsigned int chunk3Pointer = ByteConversion::fromBytes(indexP + 0, 4,
    EndiannessTypes::big, SignednessTypes::nosign);
  unsigned int vramTablesPointer = ByteConversion::fromBytes(indexP + 4, 4,
    EndiannessTypes::big, SignednessTypes::nosign);
    
  // this number is apparently not what i think it is, so we derive the
  // number of entries more directly from the total size of the chunk
//  unsigned int numVramTableEntries = ByteConversion::fromBytes(indexP + 8, 4,
//    EndiannessTypes::big, SignednessTypes::nosign);
   unsigned int numVramTableEntries
    = (chunk3Pointer - vramTablesPointer) / vramTableEntrySize;
  
  // Read VRAM entries
  vector<VramTableEntry> vramTableEntries;
  for (unsigned i = 0; i < numVramTableEntries; i++) {
    VramTableEntry entry;
    entry.fromData(indexP + vramTablesPointer + (i * vramTableEntrySize));
    vramTableEntries.push_back(entry);
  }
  
  // Clean up
  delete indexFile.buffer;
  
  // Open the image file
  BufferedFile imageFile = getFile(imageFileName.c_str());
  
  // Get pointer to image data
  char* imageP = imageFile.buffer + imageOffset;
  
  // Get compressed VRAM data size
  unsigned int vramDataSize = ByteConversion::fromBytes(imageP + 0, 4,
    EndiannessTypes::big, SignednessTypes::nosign);
  
  // Decompress VRAM data
  unsigned char vramDecompressionBuffer[vramDecompressionBufferSize];
  int decompressedVramDataSize
    = decompressMglImg((unsigned char*)(imageP + 4), vramDataSize,
                   vramDecompressionBuffer);
  
//  ofstream temp("decompressed.bin");
//  temp.write((char*)vramDecompressionBuffer, decompressedVramDataSize);
  
  // Clean up
  delete imageFile.buffer;
  
  // Create maps with all unique source addresses and palettes.
  // This step is important since the same image data may be referred to
  // multiple times in different VRAM entries, usually to color it from a
  // different palette, but we only need the unique graphics.
  map<int, VramTableEntry> uniqueSourceOffsets;
  map<int, bool> uniquePaletteOffsets;
  
  for (unsigned int i = 0; i < vramTableEntries.size(); i++) {
    uniqueSourceOffsets[vramTableEntries[i].sourceOffset()]
      = vramTableEntries[i];
    uniquePaletteOffsets[vramTableEntries[i].paletteOffset()]
      = true;
  }
  
  // Extract all unique image data.
  // In the process, enumerate the subimages and create a map so we know
  // which offset corresponds to which number
  map<int, int> imageOffsetToIndex;
  int grpnum = 0;
  for (map<int, VramTableEntry>::iterator it = uniqueSourceOffsets.begin();
       it != uniqueSourceOffsets.end();
       ++it) {
    VramTableEntry& entry = it->second;
    
    TGraphic g(entry.width(), entry.height());
    read4bppGraphicGrayscale((unsigned char*)vramDecompressionBuffer
                               + entry.sourceOffset(),
                             g, entry.width(), entry.height());
    
    imageOffsetToIndex[entry.sourceOffset()] = grpnum;
    
    string outFileName = outPath + makeImageFileName(grpnum);
    TPngConversion::graphicToRGBAPng(outFileName, g);
    
    ++grpnum;
  }
  
  // Extract all unique palettes.
  // Perform the same enumeration task as with image data
  map<int, int> paletteOffsetToIndex;
  int palnum = 0;
  for (map<int, bool>::iterator it = uniquePaletteOffsets.begin();
       it != uniquePaletteOffsets.end();
       ++it) {
    // Enumerate palette
    paletteOffsetToIndex[it->first] = palnum;
    
    string outFileName = outPath + makePaletteFileName(palnum);
    
    // Write palette to file
    ofstream ofs(outFileName.c_str(), ios_base::binary);
    ofs.write((char*)vramDecompressionBuffer + it->first, paletteSize);
    
    ++palnum;
  }
  
  // There is sometimes trailing data at the end of the data that doesn't
  // seem to be referenced by anything; it's included just in case
  if (paletteOffsetToIndex.size() > 0) {
    // Get iterator to last palette
    map<int, int>::iterator lastPaletteIt = paletteOffsetToIndex.end();
    --lastPaletteIt;
    
    int trailerOffset = lastPaletteIt->first + paletteSize;
    int trailerSize = decompressedVramDataSize - trailerOffset;
    
    ofstream ofs(trailerFileName, ios_base::binary);
    ofs.write((char*)vramDecompressionBuffer + trailerOffset, trailerSize);
  }
  else {
    // create dummy file
    ofstream ofs(trailerFileName, ios_base::binary);
  }
  
  // Write the glue file, which logs the image data -> palette associations
  // and index entries so we can reconstruct the index later
  {
    ofstream ofs((outPath + glueFileName).c_str(), ios_base::trunc);
    for (unsigned int i = 0; i < vramTableEntries.size(); i++) {
      VramTableEntry& entry = vramTableEntries[i];
      
      int paletteIndex = paletteOffsetToIndex[entry.paletteOffset()];
      int imageIndex = imageOffsetToIndex[entry.sourceOffset()];
      
      string imageName = makeImageFileName(imageIndex);
      string paletteName = makePaletteFileName(paletteIndex);
      
      GlueFileEntry glueFileEntry { (int)i, imageName, paletteName, entry };
      
      glueFileEntry.save(ofs);
    }
  }
  
/*  for (unsigned int i = 0; i < vramTableEntries.size(); i++) {
    VramTableEntry& entry = vramTableEntries[i];
    
    int colorMode = (entry.cmdpmod & 0x001C) >> 3;
    cout << i << ": " << (entry.sourceOffset()) << " " << (entry.cmdcolr * 8) << " "
         << hex << entry.cmdpmod << " " << dec << entry.width() << " " << entry.height() << endl;
    
    TGraphic g(entry.width(), entry.height());
    read4bppGraphicGrayscale((unsigned char*)vramDecompressionBuffer
                               + entry.sourceOffset(),
                             g, entry.width(), entry.height());
    
//    string outFileName = outPath + TStringConversion::toString(i) + ".png";
//    TPngConversion::graphicToRGBAPng(outFileName, g);
  } */
  
  return 0;
} 
