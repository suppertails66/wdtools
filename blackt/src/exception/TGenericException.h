#ifndef TGENERICEXCEPTION_H
#define TGENERICEXCEPTION_H


#include "exception/TException.h"
#include <string>

namespace BlackT {


/**
 * A generic exception providing an explanatory string.
 * Ideally shouldn't be used in favor of domain-specific exceptions, but
 * makes a useful placeholder during development.
 */
class TGenericException : public TException {
public:
  /**
   * Constructor.
   * @param nameOfSourceFile__ Name of the source file that triggered the
   * exception.
   * @param lineNum__ Number of the source file line at which the exception
   * was triggered.
   * @param source__ Name of the function that triggered the exception.
   * @param problem__ String explaining what caused the exception.
   */
  TGenericException(const char* nameOfSourceFile__,
                   int lineNum__,
                   const std::string& source__,
                   const std::string& problem__);
  
  /**
   * Override of std::exception::what().
   * @return C-string explaining the exception.
   */
  const char* what() const throw();
  
  /**
   * Returns a string explaining what caused the exception.
   * @return A string explaining what caused the exception.
   */
  std::string problem() const;
protected:

  /**
   * String explaining what caused the exception.
   */
  std::string problem_;
  
};


};


#endif 
