//
// Created by Hackman.Lo on 3/29/2023.
//

#include "common/validator/required_validator.h"
#include "exception/validation_exception.h"
#include <boost/format.hpp>
namespace obelisk::validator {
    void required_validator::validate() const{
      if(request_.params().contains(name_)) return;

      else {
        throw exception::validation_exception(message_);
      }
    }
} // validator