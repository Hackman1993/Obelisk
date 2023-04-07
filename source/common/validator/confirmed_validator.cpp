//
// Created by Hackman.Lo on 4/6/2023.
//

#include "common/validator/confirmed_validator.h"
#include "common/request/http_request.h"
#include "exception/validation_exception.h"
namespace obelisk {
  namespace validator {
    void confirmed_validator::validate(const std::string &name, http_request &request) {
      if(!request.params().contains(name + "_confirmation") || request.param(name) != request.param(name + "_confirmation"))
        throw exception::validation_exception(error_message({name}));
    }

    std::string confirmed_validator::error_message(std::vector<std::string> params) {
      return std::format("The {} confirmation does not match!", params[0]);
    }


    std::shared_ptr<confirmed_validator> confirmed() {
      return std::make_shared<confirmed_validator>();
    }
  } // obelisk
} // validator