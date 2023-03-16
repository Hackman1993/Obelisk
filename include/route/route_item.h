//
// Created by Hackman.Lo on 3/16/2023.
//

#ifndef OBELISK_ROUTE_ITEM_H
#define OBELISK_ROUTE_ITEM_H

#include <utility>
#include <boost/beast/http/verb.hpp>
#include <regex>
#include "route_param.h"
#include "boost/spirit/home/x3.hpp"
#include "exception/system_exception.h"
#include "request/http_response.h"
#include "request/http_request.h"
#include "utils/url/relative_url.h"

namespace obelisk {

  using namespace boost::spirit::x3;

  class route_item {

  public:

    route_item(std::string_view path, const std::function<std::unique_ptr<http_response>(http_request&)>& handler): handler_(handler){
      std::vector<std::string> splited = obelisk::utils::relative_url::split(path);
      auto router_param_parser = rule<class route_parser,router_param>{"RouterParam"} = (("{" > (+~char_("{}/") >> attr(false)) >  "}")|(+~char_("/") >> attr(true))) ;
      for(auto &item: splited){
        router_param param;
        if(!boost::spirit::x3::parse(item.begin(), item.end(), router_param_parser, param)){
          throw exception::system_exception(std::string("Router Parse Failed; Route: ").append(path));
        }
        params_.emplace_back(std::move(param));
      }
    }

    bool match(std::vector<std::string>& split){
      if(any_match_) return true;
      if(split.size()!= params_.size()) return false;

      for(int i = 0; i< params_.size(); i++)
        if(params_[i].static_ && params_[i].name_ != split[i]) return false;
      return true;
    }

    route_item& add_method(boost::beast::http::verb verb){
      available_method_.emplace(verb, true);
      return *this;
    }

    bool method_allowed(boost::beast::http::verb verb);

    std::unordered_map<std::string,std::string> parse(std::vector<std::string>& split){
      if(any_match_) return {};

      std::unordered_map<std::string, std::string> result;
      for(int i = 0; i< params_.size(); i++)
        if(!params_[i].static_) result.emplace(params_[i].name_, split[i]);
      return std::move(result);
    }

    std::unique_ptr<http_response> handle(http_request& request, std::vector<std::string>& split){
      request.route_params_ = parse(split);
      if(handler_)
        return std::move(handler_(request));
      else return nullptr;
    }



  protected:
    std::regex address_;
    std::vector<router_param> params_;
    std::vector<std::string> const_str_;
    std::unordered_map<boost::beast::http::verb, bool> available_method_;
    std::function<std::unique_ptr<http_response>(http_request&)> handler_;
    bool any_match_ = false;
  };

} // obelisk

#endif //OBELISK_ROUTE_ITEM_H
