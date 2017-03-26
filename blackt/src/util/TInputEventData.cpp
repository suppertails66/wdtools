#include "util/TInputEventData.h"

namespace BlackT {
 
 
TInputEventData::TInputEventData()
  : x_(0),
    y_(0),
    mouseLeftButton_(false),
    mouseMiddleButton_(false),
    mouseRightButton_(false),
    control_(false),
    alt_(false),
    shift_(false) { };
  
void TInputEventData
    ::morphCoordinates(int xOffset, int yOffset, double zoom) {
  x_ = (x_ / zoom) + xOffset;
  y_ = (y_ / zoom) + yOffset;
}

void TInputEventData
    ::unmorphCoordinates(int xOffset, int yOffset, double zoom) {
  x_ = (x_ - xOffset) * zoom;
  y_ = (y_ - yOffset) * zoom;
}

int TInputEventData::x() {
  return x_;
}

void TInputEventData::setX(int x__) {
  x_ = x__;
}

int TInputEventData::y() {
  return y_;
}

void TInputEventData::setY(int y__) {
  y_ = y__;
}

bool TInputEventData::mouseLeftButton() {
  return mouseLeftButton_;
}

void TInputEventData::setMouseLeftButton(bool mouseLeftButton__) {
  mouseLeftButton_ = mouseLeftButton__;
}

bool TInputEventData::mouseRightButton() {
  return mouseRightButton_;
}

void TInputEventData::setMouseRightButton(bool mouseRightButton__) {
  mouseRightButton_ = mouseRightButton__;
}

bool TInputEventData::mouseMiddleButton() {
  return mouseMiddleButton_;
}

void TInputEventData::setMouseMiddleButton(bool mouseMiddleButton__) {
  mouseMiddleButton_ = mouseMiddleButton__;
}
  
bool TInputEventData::control() {
  return control_;
}

void TInputEventData::setControl(bool control__) {
  control_ = control__;
}

bool TInputEventData::alt() {
  return alt_;
}

void TInputEventData::setAlt(bool alt__) {
  alt_ = alt__;
}

bool TInputEventData::shift() {
  return shift_;
}

void TInputEventData::setShift(bool shift__) {
  shift_ = shift__;
}


};
