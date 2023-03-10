//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_EXCEPTION_BASE_H
#define OBELISK_EXCEPTION_BASE_H
#include <string>
namespace obelisk{
  class exception_base{
  public:
    const std::string &what(){
      return what_;
    }
    exception_base(std::string str_what) : what_(std::move(str_what)){};
  protected:
    std::string what_;
  };
}

#endif //OBELISK_EXCEPTION_BASE_H
