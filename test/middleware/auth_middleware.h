//
// Created by Hackman.Lo on 7/24/2023.
//

#ifndef OBELISK_AUTH_MIDDLEWARE_H
#define OBELISK_AUTH_MIDDLEWARE_H

#include <memory>
//#include <sahara/string/string.h>
#include "common/request/http_request.h"
#include "exception/http_exception.h"
#include "common/response/http_response.h"
#include "../database.h"
#include <rosetta/rosetta.h>


class auth_middleware {
public:
    static std::shared_ptr<obelisk::http_response> handle(obelisk::http_request &request, const sahara::string &params);
};


#endif //OBELISK_AUTH_MIDDLEWARE_H
