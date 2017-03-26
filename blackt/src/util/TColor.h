#ifndef TCOLOR_H
#define TCOLOR_H


#include "util/TByte.h"

namespace BlackT {


/**
 * A color with 8-bit ARGB components.
 */
class TColor {
public:
  /**
   * Alpha value for full opacity.
   */
  const static int fullAlphaOpacity = 0xFF;
  
  /**
   * Alpha value for full transparency.
   */
  const static int fullAlphaTransparency = 0x00;

  /**
   * Default constructor.
   * Initializes to pure black (all components zero).
   */
  TColor();
  
  /**
   * Constructor.
   * Initializes to the specified color. Alpha defaults to
   * full opacity if unspecified.
   * @param r__ 8-bit red component.
   * @param g__ 8-bit green component.
   * @param b__ 8-bit blue component.
   * @param a__ 8-bit alpha component. Zero if unspecified.
   */
  TColor(TByte r__,
        TByte g__,
        TByte b__,
        TByte a__ = fullAlphaOpacity);
  
  /**
   * Array-based constructor.
   * Initializes from an array of at least 4 bytes which species the ARGB
   * components of the color in that order.
   * @param src Const pointer to array of at least 4 bytes forming ARGB color.
   */
  TColor(const TByte* src);
        
  /**
   * Getter for alpha component.
   * @return Alpha component of color.
   */
  TByte a() const;
        
  /**
   * Getter for red component.
   * @return Red component of color.
   */
  TByte r() const;
        
  /**
   * Getter for green component.
   * @return Green component of color.
   */
  TByte g() const;
        
  /**
   * Getter for blue component.
   * @return Blue component of color.
   */
  TByte b() const;
        
  /**
   * Setter for alpha component.
   * @param a__ New alpha component.
   */
  void setA(TByte a__);
        
  /**
   * Setter for red component.
   * @param r__ New red component.
   */
  void setR(TByte r__);
        
  /**
   * Setter for green component.
   * @param g__ New green component.
   */
  void setG(TByte g__);
        
  /**
   * Setter for blue component.
   * @param b__ New blue component.
   */
  void setB(TByte b__);
   
protected:

  /**
   * 8-bit alpha component.
   */
  TByte a_;
  
  /**
   * 8-bit red component.
   */
  TByte r_;
  
  /**
   * 8-bit green component.
   */
  TByte g_;
  
  /**
   * 8-bit blue component.
   */
  TByte b_;
  
};


};


#endif 
