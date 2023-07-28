//
// Created by Hackman.Lo on 3/15/2023.
//

#ifndef OBELISK_HTTP_ROUTER_H
#define OBELISK_HTTP_ROUTER_H

#include <unordered_map>
#include <regex>
#include <iostream>
#include <boost/json/object.hpp>
#include "../response/http_response.h"
#include "../request/http_request.h"
#include "../../exception/system_exception.h"
#include "route_item.h"
#include "common/response/empty_response.h"
#include "exception/http_exception.h"
#include "common/response/json_response.h"

namespace obelisk {
  class http_router {
  public:

      std::shared_ptr<http_response> prehandle(http_request& request){
          return nullptr;
      }
    std::shared_ptr<http_response> handle(http_request& request){

      for(auto &item : routers_){
          std::shared_ptr<http_response> ptr = nullptr;
          try{
              ptr = item.handle(request);
          }
          catch(exception::http_exception &e){
              ptr = std::make_shared<json_response>(boost::json::object({{"code",    e.code()},
                                                                              {"message", e.what()},
                                                                              {"data",    boost::json::value()}}), e.code());
          }
          if (ptr) {
              std::stringstream allow_origin;
              bool first = true;
              for (auto &domain: item.allow_cors()) {
                  if (first) first = false;
                  else allow_origin << ";";
                  allow_origin << domain;
              }
              ptr->add_header("Access-Control-Allow-Origin", allow_origin.str());
              ptr->add_header("Access-Control-Allow-Credentials", "true");
              ptr->add_header("Access-Control-Allow-Headers", "access-control-allow-methods,access-control-allow-origin,authorization");
              return ptr;
          }
      }
      return nullptr;
    }

    route_item& add_router(const sahara::string& path,const std::function<std::shared_ptr<http_response>(http_request&)>& handler){
        return routers_.emplace_back(path, handler);
    }
  protected:
    std::vector<route_item> routers_;
  };

} // obelisk

#endif //OBELISK_HTTP_ROUTER_H
