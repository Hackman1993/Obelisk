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
    exists_validator(const sahara::string& table, const sahara::string& field): table_(table), field_(field){}
    void validate(const sahara::string &name, http_request &request) override;

      sahara::string error_message(std::vector<sahara::string> params) override;

  protected:
      sahara::string table_;
      sahara::string field_;
  };

  std::shared_ptr<exists_validator> exists(const sahara::string& table, const sahara::string& field);

} // obelisk

#endif //OBELISK_EXISTS_VALIDATOR_H
