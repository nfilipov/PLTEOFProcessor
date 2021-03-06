#ifndef _bril_mypackage_exception_Exception_h_
#define _bril_mypackage_exception_Exception_h_
#include "xcept/Exception.h"
namespace bril { 
  namespace mypackage {
    namespace exception { 
      class Exception: public xcept::Exception{
      public: 
      Exception( std::string name, std::string message, std::string module, int line, std::string function ):xcept::Exception(name, message, module, line, function) {} 
      Exception( std::string name, std::string message, std::string module, int line, std::string function, xcept::Exception & e ):xcept::Exception(name, message, module, line, function, e) {} 
      }; 

    }//ns exception
  }//ns mypackage

  XCEPT_DEFINE_EXCEPTION(mypackage,QueueOverflow)

  XCEPT_DEFINE_EXCEPTION(mypackage,QueueEmpty)

}//ns bril
#endif
