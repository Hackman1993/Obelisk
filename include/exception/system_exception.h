//
// Created by Hackman.Lo on 3/9/2023.
//

#ifndef OBELISK_SYSTEM_EXCEPTION_H
#define OBELISK_SYSTEM_EXCEPTION_H

#include <string>
#include "exception_base.h"
namespace obelisk::exception {
  class system_exception : public exception_base {
  public:
    system_exception(const std::string& str_what): exception_base(str_what){}
  };
}


#endif //OBELISK_SYSTEM_EXCEPTION_H
