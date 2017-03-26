#ifndef MISCMATH_H
#define MISCMATH_H


namespace BlackT {


class MiscMath {
public:
  template<class T>
  static void clamp(T& value, T lower, T upper) {
    if (value < lower) {
      value = lower;
    }
    else if (value > upper) {
      value = upper;
    }
  }
  
  template<class T>
  static void wrap(T& value, T upper) {
    if (value < 0) {
      // Wrap to zero, not upper, for multiples of upper
      if (!(value % upper)) {
        value = 0;
      }
      else {
        value = (value % upper) + upper;
      }
    }
    else if (value >= upper) {
      value = (value % upper);
    }
  }
  
  static void wrapDouble(double& x_, int xLimit) {
    if (x_ >= xLimit) {
    double xrem = x_ - (int)(x_);
    int x1 = x_;
    MiscMath::wrap(x1, xLimit);
    x_ = x1;
    x_ += xrem;
    }
    else if (x_ < 0) {
      double xrem = -(x_ - (int)(x_));
      int x1 = x_;
      MiscMath::wrap(x1, xLimit);
      x_ = x1;
      x_ += xrem;
    }
  }
protected:
  
};


};


#endif 
