//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_EXCEPTION_BASE_H
#define OBELISK_EXCEPTION_BASE_H
#include <string>
namespace obelisk{
  class exception_base :public std::exception{
  public:
    explicit exception_base(std::string_view str_what) : std::exception(), message_(str_what){};
  protected:
    std::string message_;
  };
}

#endif //OBELISK_EXCEPTION_BASE_H
