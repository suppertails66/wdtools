#include "util/TColor.h"

namespace BlackT {


TColor::TColor()
 : a_(0),
   r_(0),
   g_(0),
   b_(0) { };

TColor::TColor(TByte r__,
      TByte g__,
      TByte b__,
      TByte a__)
  : a_(a__),
    r_(r__),
    g_(g__),
    b_(b__) { };

TColor::TColor(const TByte* src)
  : a_(src[3]),
    r_(src[2]),
    g_(src[1]),
    b_(src[0]) { };
        
TByte TColor::a() const {
  return a_;
}

TByte TColor::r() const {
  return r_;
}

TByte TColor::g() const {
  return g_;
}

TByte TColor::b() const {
  return b_;
}

void TColor::setA(TByte a__) {
  a_ = a__;
}

void TColor::setR(TByte r__) {
  r_ = r__;
}

void TColor::setG(TByte g__) {
  g_ = g__;
}

void TColor::setB(TByte b__) {
  b_ = b__;
}


};
