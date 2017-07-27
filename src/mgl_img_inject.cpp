#include "mgl_cmpr.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;
using namespace BlackT;

// Size of the buffer used to convert !all! of the source images to
// a data block
const static int imageConversionBufferSize = 0x100000;

// Size of the buffer used to compress the converted images
const static int imageCompressionBufferSize = 0x100000;

// Byte alignment required for start of a character table in VRAM
const static int vramCharacterTableAlignment = 0x20;

// Size of each struct in the index file
const static int indexStructSize = 0x18;

// Offset within index struct of the palette data offset
const static int indexStructCmdcolrOffset = 2;

// Offset within index struct of the source data offset
const static int indexStructCmdsrcaOffset = 4;

struct SourceImageData {
  unsigned int convertedOffset;
  int width;
  int height;
};

int main(int argc, char* argv[]) {

  if (argc < 8) {
    cout << "Mahou Gakuen Lunar! image data injector" << endl;
    cout << "Usage: " << argv[0] << "<srcdir> <imgfile>"
           << " <imgoffset> <imgdst> <ifile> <ioffset> <idst>" << endl << endl;
    cout << "  srcdir     Path to source images: 0.png, 1.png, ...\n";
    cout << "  imgfile    File containing image data to patch over.\n";
    cout << "  imgoffset  Offset of the aforementioned image data\n";
    cout << "  imgdst     Name of the patched image file to output\n";
    cout << "  ifile      Index file containing VRAM command table list\n";
    cout << "  ioffset    Offset in the file of the aforementioned list\n";
    cout << "  idst       Name of the patched index file to output\n";
    return 0;
  }
  
  string basePath(argv[1]);
  string imageFileName(argv[2]);
  unsigned int imageOffset = TStringConversion::stringToInt(argv[3]);
  string imageFileOutputName(argv[4]);
  string indexFileName(argv[5]);
  unsigned int indexOffset = TStringConversion::stringToInt(argv[6]);
  string indexFileOutputName(argv[7]);
  
  // Scan imagedir for contiguously named image files: 0.png, 1.png, ...
  // These are converted into a single contiguous block of VRAM-ready 4bpp
  // data.
  
  vector<SourceImageData> sourceImageInfo;
  unsigned int convertedImageDataSize = 0;
  
  // Prepare image conversion buffer
  unsigned char imageConversionBuffer[imageConversionBufferSize];
  std::memset(imageConversionBuffer, 0, imageConversionBufferSize);
  
  int imageCount;
  for (imageCount = 0; ; imageCount++) {
    string filename = basePath + TStringConversion::toString(imageCount)
      + ".png";
    
    // Stop at the first gap in the numbering of the files
    if (!fileExists(filename)) break;
    
    // Load image to a TGraphic
    TGraphic graphic;
    TPngConversion::RGBAPngToGraphic(filename, graphic);
    
    // Remember offset of image in converted data block
    SourceImageData info;
    info.convertedOffset = convertedImageDataSize;
    info.width = graphic.w();
    info.height = graphic.h();
    sourceImageInfo.push_back(info);
    
    // Convert to 4bpp data
    convertedImageDataSize += writeGraphic4bpp(graphic,
      imageConversionBuffer + convertedImageDataSize);
    
    // Maintain the necessary VRAM alignment by moving to the next alignment
    // boundary
    if ((convertedImageDataSize % vramCharacterTableAlignment) != 0) {
      convertedImageDataSize
        += (vramCharacterTableAlignment
            - (convertedImageDataSize % vramCharacterTableAlignment));
    }
  }
  
  // Append the palette data, if it exists
  int paletteDataOffset = -1;
  if (fileExists(basePath + "palette.bin")) {
    paletteDataOffset = convertedImageDataSize;
    BufferedFile paletteFile = getFile((basePath + "palette.bin").c_str());
    std::memcpy(imageConversionBuffer + convertedImageDataSize,
                paletteFile.buffer, paletteFile.size);
    convertedImageDataSize += paletteFile.size;
  }
  

  // Compress converted image data
  unsigned char imageCompressionBuffer[imageCompressionBufferSize];
  unsigned int compressedImageDataSize
    = compressMglImg(imageConversionBuffer,
                     convertedImageDataSize,
                     imageCompressionBuffer);
  
  // Read the source image file
  BufferedFile sourceImageFile = getFile(imageFileName.c_str());
  
  // Check that we have enough space for the new data
  unsigned int originalImageSize = ByteConversion::fromBytes(
    sourceImageFile.buffer + imageOffset, 4,
    EndiannessTypes::big, SignednessTypes::nosign);
  if (compressedImageDataSize > originalImageSize) {
    cerr << "Error: Compressed image data is too big!" << endl;
    cerr << "Original size: " << originalImageSize << endl;
    cerr << "New size: " << compressedImageDataSize
      << "(" << compressedImageDataSize - originalImageSize
      << " bytes too large)" << endl;
    return 1;
  }
  
  // Write the new image size
  ByteConversion::toBytes(compressedImageDataSize,
    sourceImageFile.buffer + imageOffset, 4,
    EndiannessTypes::big, SignednessTypes::nosign);
  
  // Write the new image data
  std::memcpy(sourceImageFile.buffer + imageOffset + 4,
              imageCompressionBuffer,
              compressedImageDataSize);
  
  // Write out the updated file
  saveFile(sourceImageFile, imageFileOutputName.c_str());
  
  // Clean up
  delete sourceImageFile.buffer;
  
  // Read the source index file
  BufferedFile sourceIndexFile = getFile(indexFileName.c_str());
  
  // Update each image's VRAM offset in the index
  for (unsigned int i = 0; i < sourceImageInfo.size(); i++) {
    unsigned int cmdsrcaPos = indexOffset + (indexStructSize * i)
      + indexStructCmdsrcaOffset;
    ByteConversion::toBytes((sourceImageInfo[i].convertedOffset) / 8,
                            sourceIndexFile.buffer + cmdsrcaPos,
                            2, EndiannessTypes::big, SignednessTypes::nosign);
    
    // Update CMDCOLR (if palette data was supplied)
    if (paletteDataOffset != -1) {
      unsigned int cmdcolrPos = indexOffset + (indexStructSize * i)
        + indexStructCmdcolrOffset;
      ByteConversion::toBytes(paletteDataOffset / 8,
                              sourceIndexFile.buffer + cmdcolrPos,
                              2, EndiannessTypes::big, SignednessTypes::nosign);
    }
  }
  
  // Write out the updated file
  saveFile(sourceIndexFile, indexFileOutputName.c_str());
  
  // Clean up
  delete sourceIndexFile.buffer;
  
//  ofstream ofs("temp.bin", ios_base::binary);
//  ofs.write((char*)imageConversionBuffer,
//              convertedImageDataSize);
  
  return 0;
  
}
