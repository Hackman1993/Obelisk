//
// Created by Hackman.Lo on 3/29/2023.
//

#ifndef OBELISK_VALIDATION_EXCEPTION_H
#define OBELISK_VALIDATION_EXCEPTION_H
#include "exception/http_exception.h"
namespace obelisk::exception {

  class validation_exception : public http_exception{
  public:
    explicit validation_exception(const std::string& what, std::size_t code = 401) : http_exception(code,what){}
  };

} // obelisk

#endif //OBELISK_VALIDATION_EXCEPTION_H
