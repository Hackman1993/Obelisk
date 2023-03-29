//
// Created by Hackman.Lo on 3/29/2023.
//

#ifndef OBELISK_REQUIRED_VALIDATOR_H
#define OBELISK_REQUIRED_VALIDATOR_H
#include "validator_base.h"
namespace obelisk::validator{

  class required_validator: public validator_base {
  public:
    required_validator(const std::string& name, http_request& request) : validator_base(name, request, name +" Is Required"){};
    void validate() const override;
  };
}

#endif //OBELISK_REQUIRED_VALIDATOR_H
