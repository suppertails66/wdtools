#include "exception/TException.h"

namespace BlackT {


TException::TException()
  : nameOfSourceFile_("unknown source file"),
    lineNum_(-1),
    source_("unknown source") { };

TException::TException(const char* nameOfSourceFile__,
                               int lineNum__,
                               const std::string& source__)
  : nameOfSourceFile_(std::string(nameOfSourceFile__)),
    lineNum_(lineNum__),
    source_(source__) { };

const std::string& TException::nameOfSourceFile() const {
  return nameOfSourceFile_;
}

int TException::lineNum() const {
  return lineNum_;
}

const std::string& TException::source() const {
  return source_;
}


} 
