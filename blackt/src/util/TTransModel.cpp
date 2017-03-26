#include "util/TTransModel.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace BlackT {


TTransModel::TTransModel()
  : w_(0),
    h_(0),
    model_(NULL) { };

TTransModel::TTransModel(const TTransModel& transModel)
  : w_(transModel.w_),
    h_(transModel.h_),
    model_(new char[transModel.w_ * transModel.h_]) {
  // Copy model data
  std::memcpy(model_, transModel.model_, size());
}

TTransModel::TTransModel(int w__, int h__)
  : w_(w__),
    h_(h__),
    model_(new char[w__ * h__]) {
  // Initialize array to transparent
  clear();
}

TTransModel& TTransModel::operator=(const TTransModel& transModel) {
  // Copy image proportions
  w_ = transModel.w_;
  h_ = transModel.h_;
  
  // Delete and reinitialize pixel data
  delete model_;
  model_ = new char[transModel.w_ * transModel.h_];
  
  // Copy model data
  std::memcpy(model_, transModel.model_, size());
  
  return *this;
}

TTransModel::~TTransModel() {
  delete model_;
}

int TTransModel::size() const {
  return (w_ * h_ * bytesPerPixel);
}

int TTransModel::bytesPerRow() const {
  return w_ * bytesPerPixel;
}

const char* TTransModel::const_model() const {
  return model_;
}

char* TTransModel::model() {
  return model_;
}
  
const char* TTransModel::const_model(int xpos, int ypos) const {
  return (model_
          + (ypos * bytesPerRow())      // get row
          + (xpos * bytesPerPixel));    // get column
}

char* TTransModel::model(int xpos, int ypos) {
  return (model_
          + (ypos * bytesPerRow())      // get row
          + (xpos * bytesPerPixel));    // get column
}

void TTransModel::copy(const TTransModel& src,
                      TRect srcbox,
                      TRect dstbox) {
/*  // Return if srcbox has a nonphysical dimension
  if ((srcbox.w() <= 0)
      || (srcbox.h() <= 0)) {
    return;
  }
  
  // Clip dstbox to fit within the model's dimensions
  dstbox.clip(TRect(0, 0, w_, h_));
  
  // Return if dstbox doesn't intersect with us
  if ((dstbox.w() <= 0)
      || (dstbox.h() <= 0)) {
    return;
  }
  
  // Get source data pointer
  const char* getpos = src.const_model()
                  + (srcbox.y() * src.bytesPerRow())  // get row
                  + (srcbox.x() * bytesPerPixel);     // get column
  
  // Get starting position to place copied data
  char* putpos = model_
                  + (dstbox.y() * bytesPerRow())      // get row
                  + (dstbox.x() * bytesPerPixel);     // get column
                  
//  const char* getpos = copyStart;
//  char* putpos = placeStart;
  
  // Copy the transparency data
  for (int j = 0; j < dstbox.h(); j++) {
    // Copy the needed portion of each row
    std::memcpy(putpos, getpos, dstbox.w() * bytesPerPixel);
    
    // Move to next row
    getpos += src.bytesPerRow();
    putpos += bytesPerRow();
  } */
  
  // Update the transparency numbering
  update(dstbox);
}

void TTransModel::blit(const TTransModel& src,
                      TRect srcbox,
                      TRect dstbox) {
/*  // Return if srcbox has a nonphysical dimension
  if ((srcbox.w() <= 0)
      || (srcbox.h() <= 0)) {
    return;
  }
  
  // Clip dstbox to fit within the model's dimensions
  dstbox.clip(TRect(0, 0, w_, h_));
  
  // Return if dstbox doesn't intersect with us
  if ((dstbox.w() <= 0)
      || (dstbox.h() <= 0)) {
    return;
  }
  
  // Get source data pointer
  const char* getpos = src.const_model()
                  + (srcbox.y() * src.bytesPerRow())  // get row
                  + (srcbox.x() * bytesPerPixel);     // get column
  
  // Get starting position to place copied data
  char* putpos = model_
                  + (dstbox.y() * bytesPerRow())      // get row
                  + (dstbox.x() * bytesPerPixel);     // get column
                  
//  const char* getpos = copyStart;
//  char* putpos = placeStart;
  
  // Copy the transparency data
  for (int j = 0; j < dstbox.h(); j++) {
    // Copy the needed portion of each row
    std::memcpy(putpos, getpos, dstbox.w() * bytesPerPixel);
    
    // Move to next row
    getpos += src.bytesPerRow();
    putpos += bytesPerRow();
  } */
  
  // Update the transparency numbering
  update(dstbox);
}

void TTransModel::update() {
  
  update(TRect(0, 0, w_, h_));
  
/*  for (int j = 0 ; j < h_; j++) {
    for (int i = 0; i < w_; i++) {
      if (*(model(i, j)) < 0) {
        std::cout << "here ";
      }
    }
    std::cout << std::endl;
  } */
}

void TTransModel::update(TRect dstbox) {
  // Clip dstbox to our area
  dstbox.clip(TRect(0, 0, w_, h_));

  // Renumber the TRect's area
  renumberArea(dstbox);
  
  // Return if there are no columns left of the renumbered area
  if (dstbox.x() == 0) {
    return;
  }
  
  // Renumber the preceding values in the rows as necessary
  minimalRenumberArea(
    TRect(0, 0, dstbox.x(), dstbox.y()));
}

void TTransModel::renumberArea(TRect dstbox) {
  // Get pointer to start of data to update
  char* putpos = model(dstbox.x(), dstbox.y());
                  
  for (int j = 0; j < dstbox.h(); j++) {
//  std::cout << j << std::endl;
    // Update each row
    renumberSegment(putpos,
                    dstbox.w());
    
    // Advance row position 
    putpos += bytesPerRow();
  }
}

void TTransModel::renumberSegment(char* putpos, int width) {
  // Get pointer to last byte
  char* last = putpos + ((width - 1) * bytesPerPixel);
    
  // Iterate over entire segment
  int pixelsDone = 0;
  while (pixelsDone < width) {
    // Determine whether counting is positive or negative
    int base;
    if (*last < 0) {
      base = transparentBaseNum;
    }
    else {
      base = solidBaseNum;
    }
    
    // Renumber data in reverse order until end of segment reached
    // or end of numbering reached
    int currentNum = base;
    
    do {
                    
//      std::cout << "*last: " << (int)(*last) << std::endl;
//      std::cout << "(*last)/(*last): " << (int)((*last)/(*last)) << std::endl;
      *(last--) = currentNum;
    
      currentNum += base;
      ++pixelsDone;
    } while ((pixelsDone < width)           // stop if start of segment reached
           && (((*last)/(std::abs(*last))) == base)   // stop if sign changes
           && (std::abs(currentNum) < 128));// stop if next would overflow
  }
}

void TTransModel::minimalRenumberArea(TRect dstbox) {
  // Get pointer to start of data to update
  char* putpos = model_
                  + (dstbox.y() * bytesPerRow())      // get row
                  + (dstbox.x() * bytesPerPixel);     // get column
                  
  for (int j = 0; j < dstbox.h(); j++) {
    // Update each row
    minimalRenumberSegment(putpos,
                    dstbox.w());
    
    // Advance row position 
    putpos += bytesPerRow();
  }
}

void TTransModel::minimalRenumberSegment(char* putpos, int width) {
  // Get pointer to last byte
  char* last = putpos + ((width - 1) * bytesPerPixel);
  
  // No work to do if |(*last)| is already 1
  if (std::abs(*last) == 1) {
    return;
  }
  
  // Determine whether counting is positive or negative
  int base;
  if (*last < 0) {
    base = transparentBaseNum;
  }
  else {
    base = solidBaseNum;
  }
  
  // Renumber data in reverse order until |1| reached
  int currentNum = base;
  int pixelsDone = 0;
  do {
    *(last--) = currentNum;
  
    currentNum += base;
    ++pixelsDone;
  } while ((pixelsDone < width)           // stop if start of segment reached
         && (std::abs(*last) != 1)        // stop if |1| reached
         && (std::abs(currentNum) < 128));// stop if next would overflow
}

void TTransModel::clear() {
  clearInternal(transparentBaseNum);
}

void TTransModel::clearTransparent() {
  clearInternal(transparentBaseNum);
}

void TTransModel::clearOpaque() {
  clearInternal(solidBaseNum);
}

void TTransModel::clearInternal(int value) {
  // Return if dimension(s) invalid
  if ((w_ <= 0)
      || (h_ <= 0)) {
    return;
  }

  // Make first row of model transparent
  
  int numBytes = w_ * bytesPerPixel;
  
  // Generate ascending sequence and place in first row in reverse
  // ... -5 -4 -3 -2 -1 -127 -126 -125 ...
  int currentNum = value;
  for (int i = numBytes - 1; i >= 0; i--) {
    model_[i] = currentNum;
    currentNum += value;
    
    // If maximum value reached, loop to base value
    if (std::abs(currentNum) >= maxRunNum) {
      currentNum = value;
    }
  }
  
  // Now, starting from the initially filled row, copy whatever
  // rows we have filled into the rows we haven't filled, using
  // ALL the currently filled rows simultaneously
  // Thus we copy one row on the first pass, two on the second,
  // four on the third, and so on until all rows are filled
  int rowsFilled = 1;
  while (rowsFilled < h_) {
    int rowsToCopy = rowsFilled;
    
    // Don't copy more rows than we have left to fill
    if (rowsToCopy > h_ - rowsFilled) {
      rowsToCopy = h_ - rowsFilled;
    }
    
    // Copy filled rows to unfilled ones
    std::memcpy(model(0, rowsFilled),
                model_,
                rowsToCopy * w_ * bytesPerPixel);
    
    // Increment fill position
    rowsFilled *= 2;
  }
}

};
