//
// Created by Hackman.Lo on 3/29/2023.
//

#ifndef OBELISK_VALIDATOR_BASE_H
#define OBELISK_VALIDATOR_BASE_H
#include "string/string.h"
#include <vector>
#include <utility>
#include <memory>
namespace obelisk{
  class http_request;
  namespace validator {


    class validator_base {
    public:
      virtual void validate(const sahara::string& name, http_request& request) = 0;
      virtual sahara::string error_message(std::vector<sahara::string> params) = 0;
    };

    struct validator_group{
        sahara::string name_;
      std::vector<std::shared_ptr<validator_base>> validators_;
    };
  }
} // validator

#endif //OBELISK_VALIDATOR_BASE_H
