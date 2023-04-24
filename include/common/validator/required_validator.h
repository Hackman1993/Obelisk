//
// Created by Hackman.Lo on 3/29/2023.
//

#ifndef OBELISK_REQUIRED_VALIDATOR_H
#define OBELISK_REQUIRED_VALIDATOR_H
#include "validator_base.h"
namespace obelisk::validator{

  class required_validator: public validator_base {
  public:
    void validate(const sahara::string& name, http_request& request) override;

      sahara::string error_message(std::vector<sahara::string> params) override;
  };

  std::shared_ptr<required_validator> required();
}

#endif //OBELISK_REQUIRED_VALIDATOR_H
