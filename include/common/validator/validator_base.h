//
// Created by Hackman.Lo on 3/29/2023.
//

#ifndef OBELISK_VALIDATOR_BASE_H
#define OBELISK_VALIDATOR_BASE_H
#include "../request/http_request.h"
namespace obelisk::validator {
  class validator_base {
  public:
    validator_base(std::string_view name, http_request& request, const std::string& default_message): name_(name), request_(request), message_(default_message){};
    virtual void validate() const = 0;

  protected:
    std::string name_;
    std::string message_;
    http_request& request_;
  };
} // validator

#endif //OBELISK_VALIDATOR_BASE_H
