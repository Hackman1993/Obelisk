//
// Created by Hackman.Lo on 3/17/2023.
//

#ifndef OBELISK_AUTH_MIDDLEWARE_H
#define OBELISK_AUTH_MIDDLEWARE_H
#include <memory>
#include "../common/request/http_request.h"
#include "../common/response/string_response.h"
namespace obelisk {

  class auth_middleware {
  public:
    static std::unique_ptr<http_response> handle(http_request& request, std::string_view params){
//      if(!params.empty())
//        std::cout << params <<std::endl;
      return nullptr;
    }

  };

} // obelisk

#endif //OBELISK_AUTH_MIDDLEWARE_H
