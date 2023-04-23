//
// Created by Hackman.Lo on 4/6/2023.
//
#include <format>
#include "DB.h"
#include "common/request/http_request.h"
#include "exception/validation_exception.h"
#include "common/validator/unique_validator.h"
namespace obelisk {
  namespace validator {
    void unique_validator::validate(const std::string &name, http_request &request) {
      std::shared_ptr<rosetta::sql_statement_base> stmt;
      if(except_field_.empty())
      {
        stmt = DB::get_connection<rosetta::database_connection_base>("default")->prepared_statement(std::format("select count({}) from {} where {}=?", field_, table_, field_));
      }
      else {
        stmt = DB::get_connection<rosetta::database_connection_base>("default")->prepared_statement(std::format("select count({}) from {} where {}=? and {}!={}", field_, table_, field_, except_field_, except_value_));
      }
      stmt->bind_param(0, request.param(name));
      auto result = stmt->execute();
      if(result->get<rosetta::unsigned_integer>(0,0) > 0)
        throw exception::validation_exception(error_message({ std::string(name)}));
    }

    std::string unique_validator::error_message(std::vector<std::string> params) {
      return std::format("The {} has already been taken!", params[0]);
    }

    std::shared_ptr<unique_validator> validator::unique(std::string_view table, std::string_view field) {
      return std::make_shared<unique_validator>(table, field);
    }
  } // obelisk
} // validator