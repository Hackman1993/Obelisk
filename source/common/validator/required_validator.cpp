//
// Created by Hackman.Lo on 3/29/2023.
//

#include "common/validator/required_validator.h"
#include "exception/validation_exception.h"
#include "common/request/http_request.h"
#include <fmt/format.h>
namespace obelisk::validator {

  void required_validator::validate(const std::string& name, http_request& request) {
    if(request.params().contains(name)) return;

    throw exception::validation_exception(error_message({ std::string(name)}));
  }

    sahara::string required_validator::error_message(std::vector<std::string> data) {
    return fmt::format("Field {} is required.", data[0]);
  }

  std::shared_ptr<required_validator> required() {
    return std::make_shared<required_validator>();
  }
} // validator