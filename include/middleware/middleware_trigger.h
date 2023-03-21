//
// Created by Hackman.Lo on 3/17/2023.
//

#ifndef OBELISK_MIDDLEWARE_TRIGGER_H
#define OBELISK_MIDDLEWARE_TRIGGER_H

#include <functional>
#include <utility>
#include "request/http_request.h"
#include "request/http_response.h"

namespace obelisk{
  using middleware_callback = std::function<std::unique_ptr<http_response> (http_request&, std::string_view)>;
  class middleware_trigger{
  public:
    middleware_trigger(std::string_view params, middleware_callback handler): handler_(std::move(handler)), params_(params){}
    std::unique_ptr<http_response> handle(http_request& request){
      if(handler_)
        return handler_(request, params_);
      return nullptr;
    }

  protected:
    std::string params_;
    middleware_callback handler_;
  };
}

#endif //OBELISK_MIDDLEWARE_TRIGGER_H