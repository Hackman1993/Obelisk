//
// Created by Hackman.Lo on 3/9/2023.
//

#ifndef OBELISK_HTTP_EXCEPTION_H
#define OBELISK_HTTP_EXCEPTION_H


#include "exception_base.h"

namespace obelisk::exception {
  class http_exception: public exception_base{
  public:
    http_exception(const std::string& what, std::size_t code = 500): exception_base(what), code_(code){};
    std::size_t code() { return code_; }
  protected:
    std::size_t code_;

  };
}


#endif //OBELISK_HTTP_EXCEPTION_H
