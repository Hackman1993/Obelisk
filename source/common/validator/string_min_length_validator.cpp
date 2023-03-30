//
// Created by Hackman.Lo on 3/29/2023.
//

#include "common/validator/string_min_length_validator.h"
#include "common/request/http_request.h"
#include "exception/validation_exception.h"
#include <boost/lexical_cast.hpp>
namespace obelisk {
  namespace validator {
    void string_min_length_validator::validate(const std::string &name, http_request &request) {
      for(auto &item: request.paramv(name)){
        if(item.length() < length_) throw exception::validation_exception(error_message({ std::string(name), boost::lexical_cast<std::string>(length_)}));
      }
    }

    std::string string_min_length_validator::error_message(std::vector<std::string> params) {

      return std::format("Field {} requires length {} at least!", params[0], params[1]);
    }

    std::shared_ptr<string_min_length_validator> min_length(std::size_t len) {
      return std::make_shared<string_min_length_validator>(len);
    }
  } // obelisk
} // validator