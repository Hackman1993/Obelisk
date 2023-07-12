//
// Created by Hackman.Lo on 3/17/2023.
//

#ifndef OBELISK_MIDDLEWARE_TRIGGER_H
#define OBELISK_MIDDLEWARE_TRIGGER_H

#include <functional>
#include <utility>
#include "../common/request/http_request.h"
#include "../common/response/http_response.h"

namespace obelisk {
    using middleware_callback = std::function<std::unique_ptr<http_response>(http_request &, const sahara::string &)>;

    class middleware_trigger {
    public:
        middleware_trigger(const sahara::string &params, middleware_callback handler) : handler_(std::move(handler)), params_(params) {}

        std::unique_ptr<http_response> handle(http_request &request) {
            if (handler_)
                return handler_(request, params_);
            return nullptr;
        }

    protected:
        sahara::string params_;
        middleware_callback handler_;
    };
}

#endif //OBELISK_MIDDLEWARE_TRIGGER_H
