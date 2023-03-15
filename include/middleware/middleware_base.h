//
// Created by Hackman.Lo on 3/15/2023.
//

#ifndef OBELISK_MIDDLEWARE_BASE_H
#define OBELISK_MIDDLEWARE_BASE_H

#include <memory>
#include "request/http_request.h"
#include "request/http_response.h"

namespace obelisk{
  class middleware_base{
    virtual std::unique_ptr<http_response> handle(http_request& request) = 0;
  };
}

#endif //OBELISK_MIDDLEWARE_BASE_H
