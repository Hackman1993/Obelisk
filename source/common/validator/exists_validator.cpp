#include "DB.h"
#include <format>
#include "common/request/http_request.h"
#include "common/validator/exists_validator.h"
#include "exception/validation_exception.h"

namespace obelisk::validator {

  void exists_validator::validate(const std::string &name, http_request &request) {
    auto stmt = DB::get_connection<rosetta::mysql_connection>("default")->prepared_statement(std::format("select count({}) as result from {} where {}=? ", field_, table_, field_));
    stmt->bind_param(0, request.param(name));
    auto result = stmt->execute();
    if(!result || result->count() <= 0 || result->get<rosetta::unsigned_integer>(0,0) == 0)
      throw exception::validation_exception(error_message({ std::string(name)}));
  }

  std::string exists_validator::error_message(std::vector<std::string> params) {
    return std::format("Field {} is not Exists!", params[0]);
  }

  std::shared_ptr<exists_validator> exists(std::string_view table, std::string_view field) {
    return std::make_shared<exists_validator>(table, field);
  }
}