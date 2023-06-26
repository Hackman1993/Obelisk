//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_EXCEPTION_BASE_H
#define OBELISK_EXCEPTION_BASE_H
#include "string/string.h"
namespace obelisk{
  class exception_base : private std::exception{
  public:
    virtual const sahara::string& message() const noexcept{
      return message_;
    }

    explicit exception_base(const std::string& str_what) : std::exception(), message_(str_what){};
  protected:
      sahara::string message_;
  };
}

#endif //OBELISK_EXCEPTION_BASE_H
