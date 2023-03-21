//
// Created by Hackman.Lo on 3/9/2023.
//

#ifndef OBELISK_NETWORK_EXCEPTION_H
#define OBELISK_NETWORK_EXCEPTION_H

#include <string>
#include "exception_base.h"

namespace obelisk::exception {
  class network_exception: public exception_base{
  public:
    network_exception(const std::string& str_what): exception_base(str_what){}

  };
}


#endif //OBELISK_NETWORK_EXCEPTION_H
