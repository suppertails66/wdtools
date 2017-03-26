#include "util/TPngConversion.h"
#include "util/TFileManip.h"
#include "exception/TGenericException.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

namespace BlackT {


#ifdef BLACKT_ENABLE_LIBPNG
  class PngFailure { };
#endif

bool TPngConversion::canConvertPng() {
  #ifdef BLACKT_ENABLE_LIBPNG
    return true;
  #else
    return false;
  #endif
}

bool TPngConversion::graphicToRGBAPng(const std::string& filename,
                      TGraphic& src) {
#ifdef BLACKT_ENABLE_LIBPNG
  if ((src.w() == 0) || (src.h() == 0)) {
    return false;
  }

  png_structp png_ptr;
  png_infop info_ptr;
  png_bytepp rows = NULL;
  
  
  try {
    // Fail if unable to allocate structures
    if (!pngwalloc(png_ptr, info_ptr)) {
      throw PngFailure();
    }
    
    png_set_IHDR(png_ptr, info_ptr,
                 src.w(), src.h(),
                 pngBitDepth_,
                 PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    // Create pixel data
    rows = new png_bytep[src.h()];
    for (int j = 0; j < src.h(); j++) {
      rows[j] = new png_byte[src.w() * TGraphic::bytesPerPixel];
      
      for (int i = 0; i < src.w(); i++) {
        TColor color = src.getPixel(i, j);
        int colpos = i * TGraphic::bytesPerPixel;
        
        rows[j][colpos + 0] = color.r();
        rows[j][colpos + 1] = color.g();
        rows[j][colpos + 2] = color.b();
        rows[j][colpos + 3] = color.a();
      }
    }
    
    // Create link to rows
    png_set_rows(png_ptr, info_ptr, rows);
    
    // Write to file
    if (!pngwrite(png_ptr, info_ptr, filename)) {
      // Fail if unable to write
      throw PngFailure();
    }
    
    // Clean up
    if (rows != NULL) {
      for (int j = 0; j < src.h(); j++) {
        delete rows[j];
      }
      delete rows;
      rows = NULL;
    }
    pngwfree(png_ptr, info_ptr);
    return true;
  }
  catch (PngFailure&) {
    if (rows != NULL) {
      for (int j = 0; j < src.h(); j++) {
        delete rows[j];
      }
      delete rows;
      rows = NULL;
    }
    pngwfree(png_ptr, info_ptr);
    return false;
  }
#else
  return false;
#endif
}
  
bool TPngConversion::RGBAPngToGraphic(const std::string& filename,
                             TGraphic& dst) {
#ifdef BLACKT_ENABLE_LIBPNG
  std::ifstream ifs(filename);
  int filesize = TFileManip::getFileSize(ifs);
  
/*  if (filesize <= 0) {
    throw TGenericException(T_SRCANDLINE,
                            "TPngConversion::RGBAPngToGraphic()",
                            "PNG filesize <= 0: "
                              + filename);
  } */
  
  if (filesize <= 0) {
    return false;
  }
  
  png_structp png_ptr;
  png_infop info_ptr;
  if (!pngralloc(png_ptr, info_ptr, filename)) {
    throw TGenericException(T_SRCANDLINE,
                            "TPngConversion::RGBAPngToGraphic()",
                            "Could not allocate for PNG read: "
                              + filename);
  }
  
  try {
  
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (bit_depth != 8) {
      throw TGenericException(T_SRCANDLINE,
                              "TPngConversion::RGBAPngToGraphic()",
                              "Unsupported PNG bit depth "
                                + TStringConversion::toString(bit_depth)
                                + ": "
                                + filename);
    }
    
    // Retrieve image width and height
//    png_read_update_info(png_ptr, info_ptr);
    int w = png_get_image_width(png_ptr, info_ptr);
    int h = png_get_image_height(png_ptr, info_ptr);
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    
    dst.resize(w, h);

//    std::cout << (int)(*(row_pointers[0])) << " " 
//               << (int)(*(row_pointers[0] + 1)) << " " 
//               << (int)(*(row_pointers[0] + 2)) << " " 
//               << (int)(*(row_pointers[0] + 3)) << " " << std::endl;

    // Copy the pixel data
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    for (int j = 0; j < h; j++) {
      png_bytep p = row_pointers[j];
      for (int i = 0; i < w; i++) {
        dst.setPixel(i, j,
                     TColor(p[2], p[1], p[0], p[3]),
                     TGraphic::transUpdate);
        p += 4;
      }
    }
  
    // Clean up
    pngrfree(png_ptr, info_ptr);
    
    return true;
  }
  catch (PngFailure&) {
    // Clean up
    pngrfree(png_ptr, info_ptr);
    
    throw TGenericException(T_SRCANDLINE,
                            "TPngConversion::RGBAPngToGraphic()",
                            "Failed reading PNG: "
                              + filename);
  }
#else
  return false;
#endif
}

/*bool TPngConversion::twoDArrayToIndexedPngGG(
                                    const std::string& filename,
                                    TTwoDByteArray& src,
                                    GGPalette palette,
                                    bool transparency) {
#ifdef BLACKT_ENABLE_LIBPNG
  if ((src.w() == 0) || (src.h() == 0)) {
    return false;
  }

  png_structp png_ptr;
  png_infop info_ptr;
  png_bytepp rows = NULL;
  
  
  try {
    // Fail if unable to allocate structures
    if (!pngwalloc(png_ptr, info_ptr)) {
      throw PngFailure();
    }
    
    png_set_IHDR(png_ptr, info_ptr,
                 src.w(), src.h(),
                 pngBitDepth_,
                 PNG_COLOR_TYPE_PALETTE,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);
                 
//    png_colorp palette = 
//      (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH
//                                * png_sizeof(png_color));
    
    // Create palette
    png_color pngPalette[GGPalette::numColorsInPalette];
//      (png_colorp)png_malloc(png_ptr, GGPalette::numColorsInPalette
//                                * png_sizeof(png_color));
    
    // Set palette colors
    for (int i = 0; i < GGPalette::numColorsInPalette; i++) {
      pngPalette[i].red = palette[i].realR();
      pngPalette[i].green = palette[i].realG();
      pngPalette[i].blue = palette[i].realB();
    }
    
    // Create link to palette
//    png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
    png_set_PLTE(png_ptr, info_ptr, pngPalette, GGPalette::numColorsInPalette);
    
    // Create and initialize transparency array
    png_byte pngTrans[GGPalette::numColorsInPalette];
    std::memset((char*)(pngTrans), 255, GGPalette::numColorsInPalette);
    
    // Set color 0 to transparent if transparency is enabled
    if (transparency) {
      pngTrans[palette.getTransColorIndex()] = 0;
    }
    
    // Create link to transparency array
    png_set_tRNS(png_ptr, info_ptr,
                 pngTrans,
                 GGPalette::numColorsInPalette,
                 NULL);

    // Create pixel data
    rows = new png_bytep[src.h()];
    for (int j = 0; j < src.h(); j++) {
      rows[j] = new png_byte[src.w()];
      
      for (int i = 0; i < src.w(); i++) {
        rows[j][i] = src.data(i, j);
      }
    }
    
    // Create link to rows
    png_set_rows(png_ptr, info_ptr, rows);
    
    // Write to file
    if (!pngwrite(png_ptr, info_ptr, filename)) {
      // Fail if unable to write
      throw PngFailure();
    }
    
    // Clean up
    if (rows != NULL) {
      for (int j = 0; j < src.h(); j++) {
        delete rows[j];
      }
      delete rows;
      rows = NULL;
    }
    pngwfree(png_ptr, info_ptr);
    return true;
  }
  catch (PngFailure&) {
    if (rows != NULL) {
      for (int j = 0; j < src.h(); j++) {
        delete rows[j];
      }
      delete rows;
      rows = NULL;
    }
    pngwfree(png_ptr, info_ptr);
    return false;
  }
#else
  return false;
#endif
}

bool TPngConversion::indexedPngToTwoDArrayGG(TTwoDByteArray& dst,
                                  const std::string& filename) {
#ifdef BLACKT_ENABLE_LIBPNG
  
  // Read file
  png_structp png_ptr;
  png_infop info_ptr;
  if (!pngralloc(png_ptr, info_ptr, filename)) {
    return false;
  }
  
  try {
  
    // Fail if image is not palettized
    if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_PALETTE) {
      throw PngFailure();
    }
    
    // Fail if data bit depth does not match expected
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    if ((bit_depth < 4) || (bit_depth > 8)) {
//      std::cout << (int)(png_get_bit_depth(png_ptr, info_ptr)) << std::endl;
      throw PngFailure();
    }
    
    png_color* palette;
    int num_palette;
    png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
    
    // Fail if there are not enough entries in the palette
    if (num_palette < GGPalette::numColorsInPalette - 1) {
//      std::cout << num_palette << std::endl;
      throw PngFailure();
    }
    
//    png_bytep trans_alpha;
//    int num_trans;
//    png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, NULL);
    
    // Retrieve image width and height
    int w = png_get_image_width(png_ptr, info_ptr);
    int h = png_get_image_height(png_ptr, info_ptr);
    
    // Initialize destination array
    dst.resize(w, h);

    // Get the pixel data
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    
    // Copy values to the destination array
    if (bit_depth == 8) {
      for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
          dst.data(i, j) = row_pointers[j][i];
        }
      }
    }
    else if (bit_depth == 4) {
      for (int j = 0; j < h; j++) {
        int xpos = 0;
        while (xpos < w) {
          Tbyte byte = row_pointers[j][xpos/2];
          Tbyte color1 = (byte & 0x0F);
          Tbyte color2 = (byte & 0xF0) >> 4;
          
          dst.data(xpos++, j) = color2;
          
          if (xpos >= w) {
            break;
          }
          
          dst.data(xpos++, j) = color1;
        }
      }
    }
    else {
      throw PngFailure();
    }
    
//    std::cout << num_palette << std::endl;
  
    // Clean up
    pngrfree(png_ptr, info_ptr);
    return true;
  }
  catch (PngFailure&) {
    // Clean up
    pngrfree(png_ptr, info_ptr);
    return false;
  }
#else
  return false;
#endif
} */

#ifdef BLACKT_ENABLE_LIBPNG
bool TPngConversion::pngralloc(
                     png_structp& png_ptr,
                     png_infop& info_ptr,
                     const std::string& filename) {
  // Generic PNG read init code, minimally modified from libpng/example.c
  
  unsigned int sig_read = 0;
//  png_uint_32 width, height;
//  int bit_depth, color_type, interlace_type;
  FILE *fp;
  if ((fp = fopen(filename.c_str(), "rb")) == NULL) {
    return false;
  }
  
  // Check if this is really a PNG
  
  char buf[pngBytesToCheck_];
  if (fread(buf, 1, pngBytesToCheck_, fp) != pngBytesToCheck_) {
    fclose(fp);
    return false;
  }
  
  if (png_sig_cmp((png_bytep)buf, (png_size_t)0, pngBytesToCheck_)) {
    fclose(fp);
    return false;
  }
  
  sig_read = pngBytesToCheck_;
  
  // Try to read the file
  
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
    NULL, NULL, NULL);
  
  if (png_ptr == NULL) {
    fclose(fp);
    return false;
  }
  
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return false;
  }
  
  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return false;
  }
  
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, sig_read);
  
  // Read entire image
//  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
  
  // Read entire image
  png_read_png(png_ptr,
               info_ptr,
//                PNG_TRANSFORM_IDENTITY
                // Attempt to standardize the read data to RGBA, regardless
                // of native format
                PNG_TRANSFORM_BGR
                  | PNG_TRANSFORM_PACKING
                  | PNG_TRANSFORM_STRIP_16
                  | PNG_TRANSFORM_EXPAND
                  | PNG_TRANSFORM_GRAY_TO_RGB 
//                  #if SDL_BYTEORDER == SDL_LITTLE_ENDIAN
//                    | PNG_TRANSFORM_PACKSWAP
//                  #endif
                ,
                NULL);
  
  fclose(fp);
  
  return true;
}
                     
bool TPngConversion::pngrfree(png_structp& png_ptr,
                    png_infop& info_ptr) {
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return true;
}

bool TPngConversion::pngwalloc(png_structp& png_ptr,
                     png_infop& info_ptr) {
  // Generic PNG write init code, minimally modified from libpng/example.c
  
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
    NULL, NULL, NULL);
    
  if (png_ptr == NULL) {
//    fclose(fp);
    return false;
  }
  
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
//    fclose(fp);
    png_destroy_write_struct(&png_ptr, NULL);
    return false;
  }
  
  return true;
}

bool TPngConversion::pngwfree(png_structp& png_ptr,
                     png_infop& info_ptr) {
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return true;
}

bool TPngConversion::pngwrite(png_structp& png_ptr,
                     png_infop& info_ptr,
                     const std::string& filename) {
  // Generic PNG write code, minimally modified from libpng/example.c
  
  FILE *fp;
  
  fp = fopen(filename.c_str(), "wb");
  if (fp == NULL) {
    return false;
  }
    
  if (setjmp(png_jmpbuf(png_ptr))) {
    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return false;
  }
  
  png_init_io(png_ptr, fp);
  
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
  
  fclose(fp);
  
  return true;
}
#endif


};
