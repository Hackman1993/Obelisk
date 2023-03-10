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
    system_exception(std::string what): exception_base(std::move(what)){}
  private:
    std::string what_;
  };
}


#endif //OBELISK_SYSTEM_EXCEPTION_H
