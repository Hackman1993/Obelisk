
#include "common/validator/string_min_length_validator.h"
#include "exception/validation_exception.h"
#include "common/request/http_request.h"
namespace obelisk::validator {

    void string_min_length_validator::validate(const std::string &name, http_request &request){
        if (request.params().contains(name)) {
            auto value = request.params().get(name);
            if (value.size() < length_) {
                throw obelisk::exception::http_exception(200, "Param " + name + " length must be greater than " + std::to_string(length_));
            }
        }
    }

    std::shared_ptr <string_min_length_validator> min_length(std::size_t len){
        return std::make_shared<string_min_length_validator>(len);
    };
}

