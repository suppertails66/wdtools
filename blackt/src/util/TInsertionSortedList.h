#ifndef TINSERTIONSORTEDLIST_H
#define TINSERTIONSORTEDLIST_H


#include <list>
#include <iostream>

namespace BlackT {


template <class T>
class TInsertionSortedList {
public:
  typedef std::list<T> NativeList;
  
  TInsertionSortedList<T>(
      bool (*sortingPredicate__)(const T, const T))
    : sortingPredicate_(sortingPredicate__) { }
  
  int size() const {
    return nativeList_.size();
  }
  
  void push_back(T t) {
    nativeList_.push_back(t);
  }
  
  typename NativeList::iterator erase(typename NativeList::iterator t) {
    return nativeList_.erase(t);
  }
  
  typename NativeList::iterator begin() {
    return nativeList_.begin();
  }
  
  typename NativeList::iterator end() {
    return nativeList_.end();
  }
  
  typename NativeList::const_iterator cbegin() const {
    return nativeList_.cbegin();
  }
  
  typename NativeList::const_iterator cend() const {
    return nativeList_.cend();
  }
  
  void sort() {
//    nativeList_.sort(sortingPredicate_);
    if (!nativeList_.size()) {
      return;
    }

    typename NativeList::iterator it1 = nativeList_.begin();
    typename NativeList::iterator it2 = nativeList_.begin();
    ++it2;
    
    while (it2 != nativeList_.end()) {
      // Second element is greater than first: move to proper location
      if (!sortingPredicate_(*it1, *it2)) {
        typename NativeList::iterator searchIt = it1;
//        while ((searchIt != nativeList_.begin())
//               && (!sortingPredicate_(*searchIt, *it2))) {
        while (true) {
          
          // Reached first element of list
          if (searchIt == nativeList_.begin()) {
            // it2 goes after first element
            if (sortingPredicate_(*searchIt, *it2)) {
              ++searchIt;
            }
            // it2 goes before first element
            else {
              
            }
            
            break;
          }
          else {
            // it2 goes after searchIt
            if (sortingPredicate_(*searchIt, *it2)) {
              ++searchIt;
              break;
            }
          }
          
          --searchIt;
        }
        
        // insert() places elements before the target, so we need to
        // increment the target position
//        ++searchIt;
        
        // Insert at correctly sorted position
        nativeList_.insert(searchIt, *it2);
        
        // Erase from former position
        nativeList_.erase(it2);
        
        // Point it2 to next element to be sorted
        it2 = it1;
        ++it2;
      }
      else {
        ++it1;
        ++it2;
      }
    }
    
/*    for (typename NativeList::iterator it = nativeList_.begin();
         it != nativeList_.end();
         ++it) { 
      std::cout << (*it)->collisionBox().x() << " ";
    }
    std::cout << std::endl; */
  }
protected:
  NativeList nativeList_;
  
  bool (*sortingPredicate_)(const T, const T);
};


}


#endif
