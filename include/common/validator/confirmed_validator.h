//
// Created by Hackman.Lo on 4/6/2023.
//

#ifndef OBELISK_CONFIRMED_VALIDATOR_H
#define OBELISK_CONFIRMED_VALIDATOR_H
#include "validator_base.h"
#include "string/string.h"

namespace obelisk {
  namespace validator {

    class confirmed_validator : public validator_base{

    public:
      void validate(const sahara::string &name, http_request &request) override;

        sahara::string error_message(std::vector<sahara::string> params) override;
    };

    std::shared_ptr<confirmed_validator> confirmed();
  } // obelisk
} // validator

#endif //OBELISK_CONFIRMED_VALIDATOR_H
