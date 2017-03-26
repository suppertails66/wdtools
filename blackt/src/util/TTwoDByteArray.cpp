#include "util/TTwoDByteArray.h"

namespace BlackT {


TTwoDByteArray::TTwoDByteArray()
  : TTwoDArray<TByte>() { };

TTwoDByteArray::TTwoDByteArray(int w__, int h__)
  : TTwoDArray<TByte>(w__, h__) { };

void TTwoDByteArray::clear() {
  for (int j = 0; j < h_; j++) {
    for (int i = 0; i < w_; i++) {
      data_[i][j] = 0;
    }
  }
}


};
