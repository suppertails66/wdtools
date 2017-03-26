#ifndef TARRAY_H
#define TARRAY_H


#include <cstring>
#include <cstdlib>
#include "util/TByte.h"
#include "util/TString.h"
#include "util/ByteConversion.h"

namespace BlackT {


/**
 * Generic not-a-vector array implementation.
 */
template <class T>
class TArray {
public:
  TArray<T>()
    : data_(NULL),
      size_(0) { };
  
  TArray<T>(unsigned int size__)
    : data_(NULL),
      size_(0) {
    resize(size__);
  }
  
  TArray<T>(unsigned int size__,
            const T& initValue)
    : data_(NULL),
      size_(0) {
    resize(size__);
    assign(initValue);
  }
  
  TArray<T>(const TArray<T>& t)
    : data_(NULL),
      size_(0) {
    copy(t);
  }
  
  TArray<T>& operator=(const TArray<T>& t) {
    copy(t);
    return *this;
  }
  
  ~TArray<T>() {
    destroyData();
  }
    
  unsigned int size() const {
    return size_;
  }
  
  void clear() {
    destroyData();
    size_ = 0;
  }
  
  bool empty() const {
    return (size_ == 0);
  }
  
  void assign(const T& value) {
    for (int i = 0; i < size_; i++) {
      data_[i] = value;
    }
  }
  
  T* data() {
    return data_;
  }
  
  const T* data() const {
    return data_;
  }
  
  T& operator[](unsigned int index) {
    return data_[index];
  }
  
  const T& operator[](unsigned int index) const {
    return data_[index];
  }
  
  void resize(unsigned int size__) {
    destroyData();
    data_ = new T[size__];
    size_ = size__;
  }
  
  // only meant to work for TArray<TByte>
  // you'd think template specialization was for this kind of stuff, but nooo
  int save(TString& dst) const {
    int byteCount = 0;
    
    TByte buffer[ByteSizes::uint32Size];
    
    ByteConversion::toBytes(size_,
                            buffer,
                            ByteSizes::uint32Size,
                            EndiannessTypes::little,
                            SignednessTypes::nosign);
    dst += TString((char*)(buffer), ByteSizes::uint32Size);
    byteCount += ByteSizes::uint32Size;
    
    for (int i = 0; i < size_; i++) {
      dst += (char)(data_[i]);
    }
    byteCount += size_;
    
    return byteCount;
  }
  
  // TArray<TByte> only
  int load(const TByte* src) {
    int byteCount = 0;
    
    int dataSize = ByteConversion::fromBytes(src + byteCount,
                            ByteSizes::uint32Size,
                            EndiannessTypes::little,
                            SignednessTypes::nosign);
    byteCount += ByteSizes::uint32Size;
    
    resize(dataSize);
    byteCount += readFromData(src + byteCount);
    
    return byteCount;
  }
  
  // TArray<TByte> only
  int readFromData(const TByte* src) const {
    std::memcpy(data_,
                src,
                size_);
    
    return size_;
  }
  
  // TArray<TByte> only
  int writeToData(TByte* dst) const {
    std::memcpy(dst,
                data_,
                size_);
    
    return size_;
  }
protected:
  void copy(const TArray<T>& t) {
    resize(t.size_);
    
    for (int i = 0; i < t.size_; i++) {
      data_[i] = t.data_[i];
    }
  }
  
  void destroyData() {
    delete[] data_;
    data_ = NULL;
  }

  T* data_;
  
  unsigned int size_;
};


};


#endif
