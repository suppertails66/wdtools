#ifndef TGRAPHIC_H
#define TGRAPHIC_H


#include "util/TByte.h"
#include "util/TTransModel.h"
#include "util/TColor.h"
#include <cstdint>

namespace BlackT {


/**
 * Storage, access, and manipulator for a raw ARGB graphic.
 * Pixel data is stored in an array in ARGB format, with 1 byte per component.
 * Pixels are ordered left-to-right, top-to-bottom.
 */
class TGraphic {
public:

  /**
   * Enum of transparency handling options for blitting.
   */
  enum TransBlitOption {
    noTransUpdate = 0,    /*< Don't update transparency model */
    transUpdate = 1       /*< Update transparency model */
  };
  
  /**
   * Enum of transparency handling options for tile-and-palette constructor.
   */
  enum TileTransferTransOption {
    noTileTrans = 0,    /*< Set alpha on pixels with transparency color */
    tileTrans = 1       /*< Leave pixels with transparency color solid */
  };

  /**
  * Number of bytes per pixel in internal pixel array.
  */
  const static int bytesPerPixel = 4;

  /**
  * Number of bits per pixel in internal pixel array.
  */
  const static int bitsPerPixel = bytesPerPixel * sizeof(TByte);
  
  /**
  * Default constructor.
  * Initializes width and height to zero and pixel array to NULL.
  */
  TGraphic();
  
  /**
  * Copy constructor.
  * @param graphic TGraphic to copy data from.
  */
  TGraphic(const TGraphic& graphic);
  
  /**
  * Copy assignment.
  * @param graphic TGraphic to copy data from.
  */
  TGraphic& operator=(const TGraphic& graphic);
  
  /**
  * Tile-and-palette constructor.
  * Given a GGTile containing color data and a GGPalette to color it with,
  * this produces a graphic corresponding to the GGTile.
  * @param tile Tile containing indexed color values.
  * @param palette Palette containing colors corresponding to the tile indices.
  */
/*  TGraphic(const GGTile& tile,
          const GGPalette& palette,
          TileTransferTransOption transOption = noTileTrans); */
  
  /**
  * Tile-and-palette constructor.
  * Given a GGTile containing color data and a GGPalette to color it with,
  * this produces a graphic corresponding to the GGTile.
  * @param tile Tile containing indexed color values.
  * @param palette Palette containing colors corresponding to the tile indices.
  */
/*  TGraphic(const GGTile& tile,
          const GGPalette& palette,
          TileTransferTransOption transOption,
          TColor backgroundColor); */
  
  /**
  * Empty graphic constructor.
  * Initializes width and height to specified values, and initializes
  * pixel array to all 0xFF (fully transparent white).
  * @param w__ TGraphic width.
  * @param h__ TGraphic height.
  */
  TGraphic(int w__, int h__);
  
  /**
  * Destructor.
  */
  ~TGraphic();
  
  int numTotalPixels() const;
  
  /**
  * Retrieves size of internal pixel data array.
  * @return Size of internal pixel data array.
  */
  int size() const;
  
  /**
  * Getter for image width.
  * @return Image width.
  */
  int w() const;
  
  /**
  * Getter for image height.
  * @return Image height.
  */
  int h() const;
  
  /**
  * Resizes image to the given size.
  * Existing contents are discarded.
  * @param w__ New image width.
  * @param h__ New image height.
  */
  void resize(int w__, int h__);
  
  /**
  * Retrieves number of bytes per row of pixel data.
  * @return Number of bytes per row of pixel data.
  */
  int bytesPerRow() const;
  
  /**
  * Returns a const pointer to the start of the internal pixel data array.
  * @return Const pointer to the start of the internal pixel data array.
  */
  const TByte* const_imgdat() const;
  
  /**
  * Returns a non-const pointer to the start of the internal pixel data array.
  * @return Non-const pointer to the start of the internal pixel data array.
  */
  TByte* imgdat();
  
  /**
  * Returns a const pointer to the internal pixel data array.
  * @param xpos X-position of data pointer.
  * @param ypos Y-position of data pointer.
  * @return Const pointer to the internal pixel data array.
  */
  const TByte* const_imgdat(int xpos, int ypos) const;
  
  /**
  * Returns a non-const pointer to the internal pixel data array.
  * @param xpos X-position of data pointer.
  * @param ypos Y-position of data pointer.
  * @return Non-const pointer to the internal pixel data array.
  */
  TByte* imgdat(int xpos, int ypos);
  
  /**
   * Copies a portion of another graphic onto this one.
   * Transparency is ignored, so alpha information will be transferred
   * instead of blended. To perform alpha blending, use blit() instead.
   * @param src The source TGraphic for the blit.
   * @param dstbox A TRect outlining the area to be blitted to in this TGraphic.
   * @param srcbox A TRect outlining the area to be blitted from src.
   * @param updateTrans If noTransUpdate, the TTransModel will not be updated.
   * Saves time if a TGraphic is to be used only as a blit destination and not a
   * source, or if it doesn't have any transparent areas.
   * @see blit()
   */
  void copy(const TGraphic& src,
            TRect dstbox,
            TRect srcbox,
            TransBlitOption updateTrans = transUpdate);
  
  /**
   * Copies a portion of another graphic onto this one.
   * Transparency is ignored, so alpha information will be transferred
   * instead of blended. To perform alpha blending, use blit() instead.
   * @param src The source TGraphic for the blit.
   * @param dstbox A TRect outlining the area to be blitted to in this TGraphic.
   * @param updateTrans If noTransUpdate, the TTransModel will not be updated.
   * Saves time if a TGraphic is to be used only as a blit destination and not a
   * source, or if it doesn't have any transparent areas.
   * @see blit()
   */
  void copy(const TGraphic& src,
            TRect dstbox,
            TransBlitOption updateTrans = transUpdate);
            
  void copyWrap(const TGraphic& src,
                TRect dstbox,
                TRect srcbox,
                TransBlitOption updateTrans = transUpdate);
  
  /**
   * Blits a portion of another graphic onto this one.
   * Similar to copy(), but uses the transparency model to avoid copying
   * transparent pixels.
   * @param src The source TGraphic for the blit.
   * @param dstbox A TRect outlining the area to be blitted to in this TGraphic.
   * If either of the TRect's w or h fields is zero, only the x and y position
   * will be used.
   * @param srcbox A TRect outlining the area to be blitted from src.
   * If either of the TRect's w or h fields is zero, only the x and y position
   * will be used.
   * @param updateTrans If noTransUpdate, the TTransModel will not be updated.
   * Saves time if a TGraphic is to be used only as a blit destination and not a
   * source, or if it doesn't have any transparent areas.
   * @see copy()
   */
  void blit(const TGraphic& src,
            TRect dstbox,
            TRect srcbox,
            TransBlitOption updateTrans = transUpdate);
  
  /**
   * Blits a portion of another graphic onto this one.
   * Uses entirety of source graphic.
   * @param src The source TGraphic for the blit.
   * @param dstbox A TRect outlining the area to be blitted to in this TGraphic.
   * If either of the TRect's w or h fields is zero, only the x and y position
   * will be used.
   * @param updateTrans If noTransUpdate, the TTransModel will not be updated.
   * Saves time if a TGraphic is to be used only as a blit destination and not a
   * source, or if it doesn't have any transparent areas.
   * @see copy()
   */
  void blit(const TGraphic& src,
            TRect dstbox,
            TransBlitOption updateTrans = transUpdate);
  
  /**
   * Blits a portion of another graphic onto this one.
   * Uses entirety of source graphic and copies to upper-left corner.
   * @param src The source TGraphic for the blit.
   * @param updateTrans If noTransUpdate, the TTransModel will not be updated.
   * Saves time if a TGraphic is to be used only as a blit destination and not a
   * source, or if it doesn't have any transparent areas.
   * @see copy()
   */
  void blit(const TGraphic& src,
            TransBlitOption updateTrans = transUpdate);
  
  /**
   * Blits a portion of another graphic onto this one.
   * Uses entirety of source graphic and copies to given (x, y) position.
   * @param src The source TGraphic for the blit.
   * @param xpos The x-position at which to blit the graphic.
   * @param ypos The y-position at which to blit the graphic.
   * @param updateTrans If noTransUpdate, the TTransModel will not be updated.
   * Saves time if a TGraphic is to be used only as a blit destination and not a
   * source, or if it doesn't have any transparent areas.
   * @see copy()
   */
  void blit(const TGraphic& src,
            int xpos,
            int ypos,
            TransBlitOption updateTrans = transUpdate);
            
  void blitWrap(const TGraphic& src,
                TRect dstbox,
                TRect srcbox,
                TransBlitOption updateTrans = transUpdate);
            
  /**
  * Returns the color of the pixel at the given position as a TColor.
  * @param xpos X-position of the target pixel.
  * @param ypos Y-position of the target pixel.
  * @return A TColor containing the pixel's ARGB values.
  */
  TColor getPixel(int xpos, int ypos);
            
  /**
  * Sets the color of a pixel.
  * If the updateTrans parameter is noTransUpdate, this function does 
  * not fully update the transparency model, leaving it in an invalid state.
  * The intention is that if a TGraphic must be modified outside of a blit()
  * or copy() -- for example, to load some new data -- it should be done
  * through calls to this function or imgdat(), then concluded with a
  * call to regenerateTransparencyModel().
  * Note that in order to facilitate the expected calls to
  * regenerateTransparencyModel() when updateTrans is noTransUpdate, the
  * transparency model is in fact modified with "stub" values indicating
  * the new pixel transparency, so any call that uses noTransUpdate should
  * consider the transparency data invalid until regenerateTransparencyModel()
  * is called.
  * @param xpos X-position of the target pixel.
  * @param ypos Y-position of the target pixel.
  * @param color A TColor containing the new ARGB values for the pixel.
  * @param updateTrans A TransBlitOption specifying whether or not to update
  * the transparency model after modifying the pixel.
  * @see imgdat()
  * @see regenerateTransparencyModel()
  */
  void setPixel(int xpos,
                int ypos,
                TColor color,
                TransBlitOption updateTrans = noTransUpdate);
                
  /**
   * Draws a line of the specified color and width between two points.
   * @param x1 X-coordinate of the starting point.
   * @param y1 Y-coordinate of the starting point.
   * @param x2 X-coordinate of the ending point.
   * @param y2 Y-coordinate of the ending point.
   * @param color TColor of the line to draw.
   * @param width Width of the line in pixels.
   * @param updateTrans A TransBlitOption specifying whether or not to update
   * the transparency model after drawing the line.
   */
  void drawLine(int x1, int y1,
                int x2, int y2,
                TColor color,
                int width,
                TransBlitOption updateTrans = transUpdate);
                
  /**
   * Draws a border of the specified color and width along a rectangular area.
   * The border extends outward from both sides of the lines forming the
   * area; for example, a lineWidth of 3 will add 1 extra row of pixels above
   * the top side of the border and 1 extra row below it, with the other
   * sides similarly augmented. Width and height may be negative.
   * Due to foolishness with the drawLine() function used to implement this,
   * lineWidth must be odd or the border will not render correctly.
   * @param x X-coordinate of the top-left corner of the border.
   * @param y Y-coordinate of the top-left corner of the border.
   * @param width Width of the bordered area.
   * @param height Height of the bordered area.
   * @param color TColor of the line to draw.
   * @param lineWidth Width of the line in pixels.
   * @param updateTrans A TransBlitOption specifying whether or not to update
   * the transparency model after drawing the border.
   */
  void drawRectBorder(int x,
                      int y,
                      int width,
                      int height,
                      TColor color,
                      int lineWidth,
                      TransBlitOption updateTrans = transUpdate);
                
  void fillRect(int x, int y,
                int width, int height,
                TColor color,
                TransBlitOption updateTrans = transUpdate);
  
  /**
   * Scales this TGraphic to another size and stores in dst.
   * The original TGraphic is unmodified.
   * The scale factor is determined by the w and h fields of dstbox; the
   * graphic will be stretched or shrunken as needed to fill the whole
   * box.
   * Scaling is done without any filtering, so scaling that is not by an even
   * factor (twice the w/h, triple...) will most likely turn out
   * poorly.
   * @param dst Destination graphic.
   * @param dstbox TRect indicating the position and size of the scaled TGraphic
   * within dst.
   * @param updateTrans A TransBlitOption specifying whether or not to update
   * the transparency model after scaling the TGraphic.
   */
  void scaleOld(TGraphic& dst,
             TRect dstbox,
             TransBlitOption updateTrans = transUpdate);
  
  /**
   * Alternate form of scale() that always fits to dst's full dimensions.
   * @param dst Destination graphic.
   * @param updateTrans A TransBlitOption specifying whether or not to update
   * the transparency model after scaling the TGraphic.
   */
  void scaleOld(TGraphic& dst,
             TransBlitOption updateTrans = transUpdate);
  
  void scale(TGraphic& src,
             TRect dstbox,
             TransBlitOption updateTrans = transUpdate);
  
  void scale(TGraphic& src,
             TransBlitOption updateTrans = transUpdate);
  
  void scaleAndBlit(TGraphic& src,
                    TRect dstbox,
                    TransBlitOption updateTrans = transUpdate);
  
  void scaleAndCopy(TGraphic& src,
                    TRect dstbox,
                    TransBlitOption updateTrans = transUpdate);
        
  /**
  * Fully regenerates the transparency model to correspond to the pixel data.
  * This is a relatively expensive call, and should generally only be used to
  * conclude setPixel() or imgdat() operations used to load new
  * graphics data.
  */
  void regenerateTransparencyModel();

  /**
   * Clear the graphic to fully transparent white.
   */
  void clear();

  /**
   * Clear the graphic to fully opaque black.
   */
  void clearOpaqueBlack();

  /**
   * Clear the graphic to fully transparent black.
   */
  void clearTransparentBlack();

  /**
   * Clear the graphic to fully transparent.
   */
  void clearTransparent();

  /**
   * Clear the graphic to the given color.
   * @param color The TColor to clear the image to.
   */
  void clear(TColor color);
  
  /**
   * Flip the graphic horizontally.
   */
  void flipHorizontal();
  
  /**
   * Flip the graphic vertically.
   */
  void flipVertical();
  
protected:

#ifdef _WIN32
  // no endianness check
#else
  union EndCheck {
    EndCheck(std::int32_t i)
      : asInt(i) { };
    std::int32_t asInt;
    TByte asChar[4];
  } endCheck = 0x01;
#endif

  enum CopyOrBlitCommand {
    blitCommand     = 0,
    copyCommand     = 1
  };

  enum CopyOrBlitWrapCommand {
    blitWrapCommand     = 0,
    copyWrapCommand     = 1
  };

  /**
   * Draws a 1-pixel-wide line of the specified color between two points.
   * @param x1 X-coordinate of the starting point.
   * @param y1 Y-coordinate of the starting point.
   * @param x2 X-coordinate of the ending point.
   * @param y2 Y-coordinate of the ending point.
   * @param color TColor of the line to draw.
   */
  void drawPixelLine(int x1, int y1,
                     int x2, int y2,
                     TColor color);
                     
  void drawHorizontalPixelLine(int x1, int y1,
                               int distance,
                               TColor color);
                     
  void scaleInternal(TGraphic& dst,
                     TRect dstbox);
  
  void scaleX(TGraphic& dst,
                 TRect dstbox,
                 int srcYPos,
                 int dstYPos);
                 
  void blitOrCopyInternal(const TGraphic& src,
                          TRect dstbox,
                          TRect srcbox,
                          TransBlitOption updateTrans = transUpdate,
                          CopyOrBlitCommand command = blitCommand);
                 
  void blitOrCopyWrapInternal(const TGraphic& src,
                          TRect dstbox,
                          TRect srcbox,
                          TransBlitOption updateTrans = transUpdate,
                          CopyOrBlitWrapCommand command = blitWrapCommand);
                 
  void copyInternal(int rowsToCopy,
                    int columnsToCopy,
                    int srcBytesPerRow,
                    int srcTTransModelBytesPerRow,
                    TByte* putpos,
                    const TByte* getpos,
                    char* transputpos,
                    const char* transgetpos);
                 
  void blitInternal(int rowsToCopy,
                    int columnsToCopy,
                    int srcBytesPerRow,
                    int srcTTransModelBytesPerRow,
                    TByte* putpos,
                    const TByte* getpos,
                    char* transputpos,
                    const char* transgetpos);
  
  /**
  * Tile-and-palette constructor.
  * Given a GGTile containing color data and a GGPalette to color it with,
  * this produces a graphic corresponding to the GGTile.
  * @param tile Tile containing indexed color values.
  * @param palette Palette containing colors corresponding to the tile indices.
  */
/*  void fromTile(const GGTile& tile,
                const GGPalette& palette,
                TileTransferTransOption transOption,
                bool skipTransparentPixels = false); */
  
  /**
  * TGraphic width.
  */
  int w_;
  
  /**
  * TGraphic height.
  */
  int h_;
  
  /**
  * Pixel data array.
  */
  TByte* imgdat_;
  
  /**
  * Transparency data.
  */
  TTransModel transModel_;
  
};


};


#endif 
