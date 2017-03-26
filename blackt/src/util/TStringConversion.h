#ifndef TSTRINGCONVERSION_H
#define TSTRINGCONVERSION_H


#include <string>
#include <sstream>

namespace BlackT {


class TStringConversion {
public:
  enum NumBaseFormat {
    baseDec,
    baseHex,
    baseOct
  };

  template<class T>
  static T fromString(const std::string& str) {
    std::istringstream iss;
    iss.str(str);
    T result;
    iss >> result;
    return result;
  }
  
  template<class T>
  static std::string toString(const T obj) {
    std::ostringstream oss;
    oss << obj;
    return oss.str();
  }
  
/*  template<class T>
  static int toInt(const T& str,
                   std::ios_base::fmtflags flags
                     = std::ios_base::dec) {
    std::istringstream iss;
    iss.flags(flags);
    iss.str(str);
    int result;
    iss >> result;
    return result;
  } */
  
  static int stringToInt(const std::string& str) {
    if (str.size() <= 0) {
      return 0;
    }
    
    // convert string to a string to eliminate leading whitespace
    std::string workstr = toString(str);
    
    std::istringstream iss;
    
    // determine correct base
    iss.unsetf(std::ios_base::basefield);
    if (workstr.size() == 1) {
      iss.setf(std::ios_base::dec);
    }
    else {
      if ((workstr[0] == '$') || (workstr[0] == '#')) {
        iss.setf(std::ios_base::hex);
        
        // stdlib doesn't recognize $ prefix for hex, so we have
        // to get rid of it
        workstr = workstr.substr(1, workstr.size() - 1);
      }
      else if (workstr[0] == '0') {
        if (workstr[1] == 'x') {
          iss.setf(std::ios_base::hex);
        }
        else {
          iss.setf(std::ios_base::dec);
        }
      }
      else {
        iss.setf(std::ios_base::dec);
      }
    }
  
    iss.str(workstr);
    int result;
    iss >> result;
    return result;
  }
  
  static std::string intToString(int val,
                                 NumBaseFormat base
                                   = baseDec) {
    std::ostringstream oss;
    
    oss.setf(std::ios_base::uppercase);
    oss.setf(std::ios_base::showbase);
    oss.unsetf(std::ios_base::basefield);
    switch (base) {
    case baseDec:
    default:
      oss.setf(std::ios_base::dec);
      break;
    case baseHex:
      oss.setf(std::ios_base::hex);
      break;
    case baseOct:
      oss.setf(std::ios_base::oct);
      break;
    }
    
    oss << val;
    
    std::string str = oss.str();
    
    // god damn
    if ((base == baseHex)
        && (oss.flags() & std::ios_base::showbase)) {
      if (str.size() >= 2) {
        str[1] = 'x';
      }
    }
    
    return str;
  }
protected:
  
};


};


#endif 
