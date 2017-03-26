#ifndef TTRANSMODEL_H
#define TTRANSMODEL_H


#include "util/TRect.h"

namespace BlackT {


/**
 * A model of an image's transparency state.
 * This class contains a 1-byte-per-pixel array indicating whether or not
 * a pixel in an image is fully transparent (i.e. its alpha component is 255).
 * Specifically, it uses a sort of unrolled-RLE8 formatting designed to speed
 * up blitting, in which values from 0 to 127 represent a number of solid
 * pixels to copy, while values from -128 to -1 represent a number of
 * transparent pixels to skip.
 */
class TTransModel {
public:
  /**
   * Number of bytes per pixel in the transparency data array.
   */
  const static int bytesPerPixel = 1;
  
  /**
   * Number of bits per pixel in the transparency data array.
   */
  const static int bitsPerPixel = bytesPerPixel * sizeof(char);
  
  /**
   * Maximum length of a run.
   */
  const static int maxRunNum = 128;
  
  /**
   * Solid base number.
   */
  const static int solidBaseNum = 1;
  
  /**
   * Transparency base number.
   */
  const static int transparentBaseNum = -1;

  /**
   * Default constructor.
   * Creates a 0 by 0 model with a NULL data array.
   */
  TTransModel();

  /**
   * Blank model constructor.
   * Creates a w__ by h__ model with a fully transparent data array.
   * @param w__ Model width.
   * @param h__ Model height.
   */
  TTransModel(int w__, int h__);
  
  /**
   * Copy constructor.
   */
  TTransModel(const TTransModel& transModel);
  
  /**
   * Copy assignment.
   */
  TTransModel& operator=(const TTransModel& transModel);
  
  /**
   * Destructor.
   */
  ~TTransModel();
  
  /**
   * Retrieves the size of the transparency data array.
   * @return Size of the transparency data array.
   */
  int size() const;
  
  /**
   * Returns the length in bytes of a row in the data array.
   * @return The length in bytes of a row in the data array.
   */
  int bytesPerRow() const;
  
  /**
   * Returns a const pointer to the start of the transparency model array.
   * @return A const pointer to the start of the transparency model array.
   */
  const char* const_model() const;
  
  /**
   * Returns a non-const pointer to the start of the transparency model array.
   * @return A non-const pointer to the start of the transparency model array.
   */
  char* model();
  
  /**
   * Returns a const pointer to a pixel in the transparency model array.
   * @param xpos X-position in the data.
   * @param ypos Y-position in the data.
   * @return A const pointer to a pixel in the transparency model array.
   */
  const char* const_model(int xpos, int ypos) const;
  
  /**
   * Returns a non-const pointer to a pixel in the transparency model array.
   * @param xpos X-position in the data.
   * @param ypos Y-position in the data.
   * @return A non-const pointer to a pixel in the transparency model array.
   */
  char* model(int xpos, int ypos);
  
  /**
   * Updates the transparency model to correspond to a copy.
   * @param src The source TTransModel for the copy.
   * @param srcbox A TRect outlining the area to be copied from src.
   * @param dstbox A TRect outlining the area to be copied to in this model.
   */
  void copy(const TTransModel& src,
            TRect srcbox,
            TRect dstbox);
  
  /**
   * Updates the transparency model to correspond to a blit.
   * @param src The source TTransModel for the blit.
   * @param srcbox A TRect outlining the area to be blitted from src.
   * @param dstbox A TRect outlining the area to be blitted to in this model.
   */
  void blit(const TTransModel& src,
            TRect srcbox,
            TRect dstbox);

  /**
   * Updates the entire transparency model to correspond to alterations.
   */
  void update();

  /**
   * Updates the transparency model to correspond to alterations in an area.
   * @param dstbox A TRect outlining the area which has been changed.
   */
  void update(TRect dstbox);

  /**
   * Clear the model to fully transparent status.
   */
  void clear();

  /**
   * Clear the model to fully transparent status.
   */
  void clearTransparent();

  /**
   * Clear the model to fully opaque status.
   */
  void clearOpaque();
   
protected:

  void clearInternal(int value);

  /**
   * The width of the image this model corresponds to.
   */
  int w_;

  /**
   * The height of the image this model corresponds to.
   */
  int h_;

  /**
   * The transparent model array.
   * This is a 1-byte-per-pixel array indicating whether or not
   * a pixel in an image is fully transparent (i.e. its alpha component is 255).
   * Specifically, it uses a sort of unrolled-RLE8 formatting designed to speed
   * up blitting, in which values from 0 to 127 represent a number of solid
   * pixels to copy, while values from -128 to -1 represent a number of
   * transparent pixels to skip.
   */
  char* model_;

  /**
   * Renumbers an area of the transparency data.
   * @param dstbox A TRect outlining the area which has been changed.
   */
  void renumberArea(TRect dstbox);

  /**
   * Renumbers a segment of a particular row of the transparency data.
   * @param putpos Pointer to the start of the segment to be renumbered.
   * @param width The number of pixels of the segment to renumber.
   */
  void renumberSegment(char* putpos, int width);

  /**
   * Renumbers an area of the transparency data.
   * @param dstbox A TRect outlining the area which has been changed.
   */
  void minimalRenumberArea(TRect dstbox);

  /**
   * Renumbers a segment of a particular row of the transparency data.
   * Only renumbers until a |1| is reached.
   * @param putpos Pointer to the start of the segment to be renumbered.
   * @param width The maximum number of pixels of the segment to renumber.
   */
  void minimalRenumberSegment(char* putpos, int width);
  
};


};


#endif 
