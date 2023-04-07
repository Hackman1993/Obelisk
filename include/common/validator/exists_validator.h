//
// Created by Hackman.Lo on 4/6/2023.
//

#ifndef OBELISK_EXISTS_VALIDATOR_H
#define OBELISK_EXISTS_VALIDATOR_H
#include <string>
#include "validator_base.h"
namespace obelisk::validator {

  class exists_validator : public validator_base {
  public:
    exists_validator(std::string_view table, std::string_view field): table_(table), field_(field){}
    void validate(const std::string &name, http_request &request) override;

    std::string error_message(std::vector<std::string> params) override;

  protected:
    std::string table_;
    std::string field_;
  };

  std::shared_ptr<exists_validator> exists(std::string_view table, std::string_view field);

} // obelisk

#endif //OBELISK_EXISTS_VALIDATOR_H
