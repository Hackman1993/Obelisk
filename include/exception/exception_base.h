//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_EXCEPTION_BASE_H
#define OBELISK_EXCEPTION_BASE_H
#include <string>
namespace obelisk{
  class exception_base :public std::exception{
  public:
    exception_base(const std::string& str_what) : std::exception(){};
  protected:
  };
}

#endif //OBELISK_EXCEPTION_BASE_H
