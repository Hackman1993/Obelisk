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
#include "exception/system_exception.h"

namespace obelisk {
  using namespace boost::spirit::x3;
  auto router_param_parser = rule<class route_parser,router_param>{"RouterParam"} = ("{" > (+~char_("{}/") >> attr(false)) >  "}")|(+~char_("/") >> attr(true)) ;
  class router_item {

  public:

    router_item(std::string_view path){
      if(path == "*"){
        any_match_ = true;
        return;
      }
      if(path.front() != '/')
        throw exception::system_exception(std::string("Router Should Begin With /; Route: ").append(path));
      if(!boost::spirit::x3::parse(path.begin(), path.end(),"/" >> router_param_parser%"/" ,params_))
        throw exception::system_exception(std::string("Invalid Route Path; Route: ").append(path));
    }

    bool match(std::vector<std::string>& split){
      if(any_match_) return true;
      if(split.size()!= params_.size()) return false;

      for(int i = 0; i< params_.size(); i++)
        if(params_[i].static_ && params_[i].name_ != split[i]) return false;
      return true;
    }

    std::unordered_map<std::string,std::string> parse(std::vector<std::string>& split){
      if(any_match_) return {};

      std::unordered_map<std::string, std::string> result;
      for(int i = 0; i< params_.size(); i++)
        if(!params_[i].static_) result.emplace(params_[i].name_, split[i]);
      return std::move(result);
    }


  protected:
    std::regex address_;
    std::vector<router_param> params_;
    std::vector<std::string> const_str_;
    std::unordered_map<boost::beast::http::verb, bool> available_method_;
    std::function<std::unique_ptr<http_response> (http_request)> handler_;
    bool any_match_ = false;
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
