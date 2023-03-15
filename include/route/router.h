//
// Created by Hackman.Lo on 3/15/2023.
//

#ifndef OBELISK_ROUTER_H
#define OBELISK_ROUTER_H

#include <unordered_map>
#include <boost/beast.hpp>
#include <regex>
#include "request/http_response.h"
#include "request/http_request.h"
#include "boost/spirit/home/x3.hpp"
#include "route_param.h"
namespace obelisk {
  using namespace boost::spirit::x3;
  auto router_param_parser = rule<class route_parser,router_param>{"RouterParam"} = ("{" > +~char_("{}/?") > (("?">> attr(false))|attr(true)) > "}");
  class router_item {

  public:

    router_item(std::string_view path){
      bool result = boost::spirit::x3::parse(path.begin(), path.end(),(+~char_("{}"))% router_param_parser,const_str_);
      bool result3 = boost::spirit::x3::parse(path.begin(), path.end(),omit[*~char_("{}")] >>router_param_parser%(+~char_("{}")) ,params_);

      result = false;
    }

    auto matcher(){

      for(int i = 0; i < std::max<std::size_t>(params_.size(), const_str_.size()); ++i)
      {

      }
      return
    }


  protected:
    std::regex address_;
    std::vector<router_param> params_;
    std::vector<std::string> const_str_;
    std::unordered_map<boost::beast::http::verb, bool> available_method_;
    std::function<std::unique_ptr<http_response> (http_request)> handler_;
  };

  class router {
  public:
    bool try_match(std::string_view address){
      return false;
    }
  protected:
    std::vector<router_item> routers_;
  };

} // obelisk

#endif //OBELISK_ROUTER_H
