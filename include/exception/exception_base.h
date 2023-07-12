//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_EXCEPTION_BASE_H
#define OBELISK_EXCEPTION_BASE_H
#include <sahara/string/string.h>
namespace obelisk{
  class exception_base : public std::exception{
  public:
      const char* what() const noexcept override{
          return message_.c_str();
      }

    explicit exception_base(const std::string& str_what) : std::exception(), message_(str_what){};
  protected:
      std::string message_;
  };
}

#endif //OBELISK_EXCEPTION_BASE_H
