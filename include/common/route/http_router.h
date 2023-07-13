//
// Created by Hackman.Lo on 3/15/2023.
//

#ifndef OBELISK_HTTP_ROUTER_H
#define OBELISK_HTTP_ROUTER_H

#include <unordered_map>
#include <regex>
#include <iostream>
#include "../response/http_response.h"
#include "../request/http_request.h"
#include "../../exception/system_exception.h"
#include "route_item.h"

namespace obelisk {
  class http_router {
  public:

      std::unique_ptr<http_response> prehandle(http_request& request){
          return nullptr;
      }
    std::unique_ptr<http_response> handle(http_request& request){
      std::vector<sahara::string> split_path = request.path().split("/");
      auto method = request.method();
      for(auto &item : routers_){
        if(! item.match(split_path)) continue;
        if(method.iequals("OPTION") || method.iequals("HEAD")){
          return nullptr;//std::make_unique<empty_response>();
        }
        if(!item.method_allowed(method))
          return nullptr;//std::make_unique<string_response>(405, "Method Not Allowed!");

        for(auto & middleware: item.get_middlewares()){
          auto resp = middleware.handle(request);
          if(resp) return resp;
        }

        return std::move(item.handle(request, split_path));
      }
      return nullptr;
    }

    route_item& add_router(const sahara::string& path,const std::function<std::unique_ptr<http_response>(http_request&)>& handler){
        return routers_.emplace_back(path, handler);
    }
  protected:
    std::vector<route_item> routers_;
  };

} // obelisk

#endif //OBELISK_HTTP_ROUTER_H
