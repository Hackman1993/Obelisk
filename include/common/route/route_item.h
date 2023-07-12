//
// Created by Hackman.Lo on 3/16/2023.
//

#ifndef OBELISK_ROUTE_ITEM_H
#define OBELISK_ROUTE_ITEM_H

#include <utility>
#include <regex>
#include "route_param.h"
#include "boost/spirit/home/x3.hpp"
#include "exception/system_exception.h"
#include "common/response/http_response.h"
#include "common/request/http_request.h"
#include "middleware/middleware_trigger.h"

namespace obelisk {

  using namespace boost::spirit::x3;

  class route_item {

  public:
    route_item(const sahara::string& path, std::function<std::unique_ptr<http_response>(http_request&)>  handler);

    bool match(std::vector<sahara::string>& split);
    bool method_allowed(const sahara::string& method);
    std::vector<middleware_trigger>& get_middlewares();
    route_item& add_method(const sahara::string& method);
    route_item& middleware(const sahara::string& params, middleware_callback trigger);
    std::unordered_map<sahara::string,sahara::string> parse(std::vector<sahara::string>& split);
    std::unique_ptr<http_response> handle(http_request& request, std::vector<sahara::string>& split);


  protected:
    std::regex address_;
    bool any_match_ = false;
    std::vector<router_param> params_;
    std::vector<std::string> const_str_;
    std::vector<middleware_trigger> middlewares_;
    std::unordered_map<sahara::string, bool> available_method_;
    std::function<std::unique_ptr<http_response>(http_request&)> handler_;
  };

} // obelisk

#endif //OBELISK_ROUTE_ITEM_H
