//
// Created by Hackman.Lo on 4/6/2023.
//

#ifndef OBELISK_UNIQUE_VALIDATOR_H
#define OBELISK_UNIQUE_VALIDATOR_H
#include <string>
#include "validator_base.h"
#include <boost/lexical_cast.hpp>
namespace obelisk::validator {

    class unique_validator : public validator_base {
    public:
      unique_validator(std::string_view table, std::string_view field, std::string_view except_field = "", const std::string& except_value_ = ""): table_(table), field_(field), except_field_(except_field), except_value_(except_value_.empty()? "":"'"+ except_value_ + "'"){}
      unique_validator(std::string_view table, std::string_view field, std::string_view except_field, std::uint32_t except_value_): table_(table), field_(field), except_field_(except_field), except_value_(boost::lexical_cast<std::string>(except_value_)){}
      unique_validator(std::string_view table, std::string_view field, std::string_view except_field, std::int32_t except_value_): table_(table), field_(field), except_field_(except_field), except_value_(boost::lexical_cast<std::string>(except_value_)){}
      unique_validator(std::string_view table, std::string_view field, std::string_view except_field, std::int64_t except_value_): table_(table), field_(field), except_field_(except_field), except_value_(boost::lexical_cast<std::string>(except_value_)){}
      unique_validator(std::string_view table, std::string_view field, std::string_view except_field, std::uint64_t except_value_): table_(table), field_(field), except_field_(except_field), except_value_(boost::lexical_cast<std::string>(except_value_)){}

      void validate(const std::string &name, http_request &request) override;

      std::string error_message(std::vector<std::string> params) override;
    protected:
      std::string table_;
      std::string field_;
      std::string except_field_;
      std::string except_value_;
    };

  std::shared_ptr<unique_validator> unique(std::string_view table, std::string_view field);

    template <typename T>
    std::shared_ptr<unique_validator> unique(std::string_view table, std::string_view field, std::string except_field, T except_value){
      return std::make_shared<unique_validator>(table, field, except_field, except_value);
    };
}

#endif //OBELISK_UNIQUE_VALIDATOR_H
