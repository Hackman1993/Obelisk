#include "common/validator/required_validator.h"
#include <memory>
#include "common/request/http_request.h"
#include "exception/http_exception.h"
namespace obelisk::validator {
    std::shared_ptr<required_validator> required() {
        return std::make_shared<required_validator>();
    }

    void required_validator::validate(const std::string &name, http_request &request) {
        if (!request.params().contains(name)) {
            throw obelisk::exception::http_exception("Required param " + name + " not found", 200);
        }
    }
}