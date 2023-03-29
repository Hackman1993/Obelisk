//
// Created by Hackman.Lo on 3/29/2023.
//

#ifndef OBELISK_VALIDATION_EXCEPTION_H
#define OBELISK_VALIDATION_EXCEPTION_H
#include "exception/http_exception.h"
namespace obelisk::exception {

  class validation_exception : public http_exception{
  public:
    explicit validation_exception(std::string_view what, std::size_t code = 401) : http_exception(what, code){}

  };

} // obelisk

#endif //OBELISK_VALIDATION_EXCEPTION_H
