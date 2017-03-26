#include "util/TRect.h"
#include "util/ByteConversion.h"
#include "util/TSerialize.h"
#include <iostream>

namespace BlackT {


TRect::TRect()
  : x_(0),
    y_(0),
    w_(0),
    h_(0) { };

TRect::TRect(int x__, int y__, int w__, int h__)
  : x_(x__),
    y_(y__),
    w_(w__),
    h_(h__) { };

int TRect::x() const {
  return x_;
}

int TRect::y() const {
  return y_;
}

int TRect::w() const {
  return w_;
}

int TRect::h() const {
  return h_;
}
  
void TRect::setX(int x__) {
  x_ = x__;
}

void TRect::setY(int y__) {
  y_ = y__;
}

void TRect::setW(int w__) {
  w_ = w__;
}

void TRect::setH(int h__) {
  h_ = h__;
}


void TRect::clip(const TRect& srcbox) {
  // Clip in X
  clipAxis(x_, w_, srcbox.x(), srcbox.w());
  
  // Clip in Y
  clipAxis(y_, h_, srcbox.y(), srcbox.h());
}
  
bool TRect::pointOverlaps(int pointX, int pointY) {
  if (pointX < x_) { return false; }
  if (pointY < y_) { return false; }
  if (pointX >= (x_ + w_)) { return false; }
  if (pointY >= (y_ + h_)) { return false; }
  
  return true;
}
  
bool TRect::boxOverlaps(TRect box) {
  if (pointOverlaps(box.x(), box.y())) { return true; }
  if (pointOverlaps(box.x() + box.w(), box.y())) { return true; }
  if (pointOverlaps(box.x(), box.y() + box.h())) { return true; }
  if (pointOverlaps(box.x() + box.w(), box.y() + box.h())) { return true; }
  
  if (box.pointOverlaps(x_, y_)) { return true; }
  if (box.pointOverlaps(x_ + w_, y_)) { return true; }
  if (box.pointOverlaps(x_, y_ + h_)) { return true; }
  if (box.pointOverlaps(x_ + w_, y_ + h_)) { return true; }
  
  return false;
}

/*void TRect::clipX(const TRect& srcbox) {
  // Get the x-position of the right edge of srcbox
  int srcboxR = srcbox.x() + srcbox.w();
  
  // Get the x-position of the right edge of this box
  int thisboxR = x_ + w_;
  
  // If this box's width is wholly enclosed by the box's, there's nothing to do
  if ((srcbox.x() <= x_)
      && (srcboxR > thisboxR)) {
    return;
  }
  
  // If right side of srcbox does not clip, no intersection
  if (srcboxR < x_) {
    x_ = 0;
    w_ = 0;
    return;
  }
  
  // If left side of srcbox does not clip, no intersection
  if (srcbox.x() >= thisboxR) {
    x_ = 0;
    w_ = 0;
    return;
  }
  
  // If srcbox's left side is left of this box's, clip right side of this box
  if (srcbox.x() <= x_) {
    // X position remains the same, but width decreases
    // by the distance between the right edges of the boxes
    w_ -= (srcboxR - thisboxR);
  }
  // If srcbox's left side is right of this box's, clip left side of this box
  else {
    
    // Remember number of pixels to remove from left side
    int leftPixelsToClip = srcbox.x() - x_;
    // Move left side of box to match srcbox
    x_ = srcbox.x();
    // Decrease width accordingly
    w_ -= leftPixelsToClip;
    
    // Compute the number of pixels to clip from the right side of this box
    int rightPixelsToClip = thisboxR - srcboxR;
    
    // Never increase the size of this box
    if (rightPixelsToClip <= 0) {
      return;
    }
    
    // Clip right side of this box
    w_ -= rightPixelsToClip;
  }
}

void TRect::clipY(const TRect& srcbox) {

} */


void TRect::clipAxis(int& axis,
                   int& length,
                   int srcAxis,
                   int srcLength) {
  // Get the x-position of the right edge of srcbox
  int srcboxR = srcAxis + srcLength;
  
  // Get the x-position of the right edge of this box
  int thisboxR = axis + length;
  
  // If this box's width is wholly enclosed by the box's, there's nothing to do
  if ((srcAxis <= axis)
      && (srcboxR > thisboxR)) {
    return;
  }
  
  // If right side of srcbox does not clip, no intersection
  if (srcboxR < axis) {
    axis = 0;
    length = 0;
    return;
  }
  
  // If left side of srcbox does not clip, no intersection
  if (srcAxis >= thisboxR) {
    axis = 0;
    length = 0;
    return;
  }
  
//   std::cout << "axis: " << axis << std::endl;
//   std::cout << "length: " << length << std::endl;
//   std::cout << "srcAxis: " << srcAxis << std::endl;
//   std::cout << "srcLength: " << srcLength << std::endl;
//   std::cout << "thisboxR: " << thisboxR << std::endl;
//   std::cout << "srcboxR: " << srcboxR << std::endl;
  
  // If srcbox's left side is left of this box's, clip right side of this box
  if (srcAxis <= axis) {
//    std::cout << "clip distance: " << (thisboxR - srcboxR) << std::endl;
    // X position remains the same, but width decreases
    // by the distance between the right edges of the boxes
    length -= (thisboxR - srcboxR);
  }
  // If srcbox's left side is right of this box's, clip left side of this box
  else {
    
    // Remember number of pixels to remove from left side
    int leftPixelsToClip = srcAxis - axis;
    // Move left side of box to match srcbox
    axis = srcAxis;
    // Decrease width accordingly
    length -= leftPixelsToClip;
    
//    std::cout << "leftPixelsToClip: " << leftPixelsToClip << std::endl;
//    std::cout << "axis: " << axis << std::endl;
//    std::cout << "length: " << length << std::endl;
    
    // Compute the number of pixels to clip from the right side of this box
    int rightPixelsToClip = thisboxR - srcboxR;
    
    // Never increase the size of this box
    if (rightPixelsToClip <= 0) {
      return;
    }
    
    // Clip right side of this box
    length -= rightPixelsToClip;
  }
}
  
/*int TRect::read(const char* data) {
  x_ = ByteConversion::fromBytes(data + 0, 2,
                          EndiannessTypes::little, SignednessTypes::sign);
  y_ = ByteConversion::fromBytes(data + 2, 2,
                          EndiannessTypes::little, SignednessTypes::sign);
  w_ = ByteConversion::fromBytes(data + 4, 2,
                          EndiannessTypes::little, SignednessTypes::nosign);
  h_ = ByteConversion::fromBytes(data + 6, 2,
                          EndiannessTypes::little, SignednessTypes::nosign);
  
  return serializedSize;
} */
  
void TRect::read(std::istream& ifs) {
  x_ = TSerialize::readInt(ifs, TSerialize::u16size,
                           EndiannessTypes::little,
                           SignednessTypes::sign);
  y_ = TSerialize::readInt(ifs, TSerialize::u16size,
                           EndiannessTypes::little,
                           SignednessTypes::sign);
  w_ = TSerialize::readInt(ifs, TSerialize::u16size,
                           EndiannessTypes::little,
                           SignednessTypes::sign);
  h_ = TSerialize::readInt(ifs, TSerialize::u16size,
                           EndiannessTypes::little,
                           SignednessTypes::sign);
}

void TRect::write(std::ostream& ofs) {
  char buffer[2];
  
  ByteConversion::toBytes(x_, buffer, 2,
                          EndiannessTypes::little, SignednessTypes::sign);
  ofs.write(buffer, 2);
  ByteConversion::toBytes(y_, buffer, 2,
                          EndiannessTypes::little, SignednessTypes::sign);
  ofs.write(buffer, 2);
  ByteConversion::toBytes(w_, buffer, 2,
                          EndiannessTypes::little, SignednessTypes::nosign);
  ofs.write(buffer, 2);
  ByteConversion::toBytes(h_, buffer, 2,
                          EndiannessTypes::little, SignednessTypes::nosign);
  ofs.write(buffer, 2);
  
//  return serializedSize;
}
  
void TRect::makeDimensionsValid() {
  if (w_ < 0) {
    x_ += w_;
    w_ = -w_;
  }
  
  if (h_ < 0) {
    y_ += h_;
    h_ = -h_;
  }
}



}; 
