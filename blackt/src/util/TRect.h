#ifndef TRECT_H
#define TRECT_H


#include <iostream>

namespace BlackT {


/**
 * Class representing a box with an x-position, y-position, width, and height.
 */
class TRect {
public:
  const static int serializedSize = 8;
  
  /** 
   * Default constructor.
   * Initializes all field to 0.
   */
  TRect();
  
  /** 
   * Constructor.
   * Creates a box of the specified x- and y-positions, width, and height.
   * @param x__ TRect x-position.
   * @param y__ TRect y-position.
   * @param w__ TRect width.
   * @param w__ TRect height.
   */
  TRect(int x__, int y__, int w__, int h__);
  
  /** 
   * X-position getter.
   * @return TRect x-position.
   */
  int x() const;
  
  /** 
   * Y-position getter.
   * @return TRect y-position.
   */
  int y() const;
  
  /** 
   * Width getter.
   * @return TRect width.
   */
  int w() const;
  
  /** 
   * Height getter.
   * @return TRect height.
   */
  int h() const;
  
  void setX(int x__);
  void setY(int y__);
  void setW(int w__);
  void setH(int h__);
  
  /** 
   * Clips this box to only the area intersecting with another box.
   * @param srcbox The TRect used as reference for clipping this one.
   */
  void clip(const TRect& srcbox);
  
  bool pointOverlaps(int pointX, int pointY);
  
  bool boxOverlaps(TRect box);
  
//  int read(const char* data);
  
  void read(std::istream& ifs);
  
  void write(std::ostream& ofs);
  
  /**
   * If width and/or height are negative, this adjusts the position and
   * dimensions of the box to ensure width and height are positive.
   */
  void makeDimensionsValid();
  
protected:

  /** 
   * TRect x-position.
   */
  int x_;

  /** 
   * TRect y-position.
   */
  int y_;

  /** 
   * TRect width.
   */
  int w_;

  /** 
   * TRect height.
   */
  int h_;
  
  /** 
   * Clips this box's x and w to only the area intersecting with another box.
   * @param srcbox The TRect used as reference for clipping this one.
   */
//  void clipX(const TRect& srcbox);
  
  /** 
   * Clips this box's y and h to only the area intersecting with another box.
   * @param srcbox The TRect used as reference for clipping this one.
   */
//  void clipY(const TRect& srcbox); 
  
  /** 
   * Generic function to clip one of a box's axes.
   * @param axis Reference to the axis to update (x or y position).
   * @param length Reference to the length to update (width or height).
   * @param srcAxis The value of the axis for comparison (x or y position).
   * @param srcLength The value of the length for comparison (width or height).
   */
  void clipAxis(int& axis,
                int& length,
                int srcAxis,
                int srcLength);
  
};


};


#endif 
