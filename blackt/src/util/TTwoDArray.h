#ifndef TTWODARRAY_H
#define TTWODARRAY_H


#include "exception/TGenericException.h"
#include "util/TByte.h"
#include "util/TStringConversion.h"
#include <cstdlib>

namespace BlackT {


template <class T>
class TTwoDArray {
public:
  TTwoDArray<T>()
    : data_(NULL),
      w_(0),
      h_(0) { };
  
  TTwoDArray<T>(int w__, int h__)
    : data_(NULL),
      w_(w__),
      h_(h__) {
    reinitializeArray(w__, h__);
  }
  
  TTwoDArray<T>(const TTwoDArray<T>& t)
    : data_(NULL),
      w_(t.w_),
      h_(t.h_) {
    cloneIntoThis(t);
  }
  
  TTwoDArray<T>& operator=(const TTwoDArray<T>& t) {
    cloneIntoThis(t);
    
    return *this;
  }
  
  ~TTwoDArray<T>() {
    destroyArray();
  }
  
  void resize(int w__, int h__) {
    reinitializeArray(w__, h__);
  }
    
  int w() const {
    return w_;
  }
  
  int h() const {
    return h_;
  }
  
  T& data(int x, int y) {
    if ((x < 0) || (x >= w_)) {
      throw TGenericException(T_SRCANDLINE,
                              "TTwoDArray::data(int,int) [x]",
                              "Out-of-range x-index: "
                                + TStringConversion::toString(x));
    }
    
    if ((y < 0) || (y >= h_)) {
      throw TGenericException(T_SRCANDLINE,
                              "TTwoDArray::data(int,int) [y]",
                              "Out-of-range y-index: "
                                + TStringConversion::toString(y));
    }
  
    return data_[x][y];
  }
  
  const T& data(int x, int y) const {
    if ((x < 0) || (x >= w_)) {
      throw TGenericException(T_SRCANDLINE,
                              "TTwoDArray::data(int,int) const [x]",
                              "Out-of-range x-index: "
                                + TStringConversion::toString(x));
    }
    
    if ((y < 0) || (y >= h_)) {
      throw TGenericException(T_SRCANDLINE,
                              "TTwoDArray::data(int,int) const [y]",
                              "Out-of-range y-index: "
                                + TStringConversion::toString(y));
    }
    
    return data_[x][y];
  }
  
  void setDataClipped(int x, int y, const T& t) {
    if ((x < 0)
        || (x >= w_)
        || (y < 0)
        || (y >= h_)) {
      return;
    }
  
    data_[x][y] = t;
  }
  
  void fill(const T& value) {
    for (int j = 0; j < h_; j++) {
      for (int i = 0; i < w_; i++) {
        data_[i][j] = value;
      }
    }
  }
protected:
  void cloneIntoThis(const TTwoDArray& t) {
//    if ((w_ != t.w_) || (h_ != t.h_)) {
      reinitializeArray(t.w_, t.h_);
//    }
    
    for (int i = 0; i < t.w_; i++) {
      for (int j = 0; j < t.h_; j++) {
        data_[i][j] = t.data_[i][j];
      }
    }
  }

  void destroyArray() {
    if (data_ == NULL) {
      return;
    }
    
    for (int i = 0; i < w_; i++) {
      delete[] data_[i];
    }
    
    delete[] data_;
  }

  void reinitializeArray(int w__, int h__) {
    destroyArray();
    
    data_ = new T*[w__];
    
    for (int i = 0; i < w__; i++) {
      data_[i] = new T[h__];
    }
  
    w_ = w__;
    h_ = h__;
  }

  T** data_;
  
  int w_;
  int h_;
};


};


#endif
