#ifndef TTWODBYTEARRAY_H
#define TTWODBYTEARRAY_H


#include "util/TTwoDArray.h"
#include "util/TByte.h"

namespace BlackT {


class TTwoDByteArray : public TTwoDArray<TByte> {
public:
  TTwoDByteArray();
  TTwoDByteArray(int w__, int h__);
  
  void clear();
protected:
  
};


};


#endif
