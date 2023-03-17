//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_HTTP_REQUEST_H
#define OBELISK_HTTP_REQUEST_H
#include <boost/beast.hpp>
#include "http_header.h"

namespace obelisk{
  class http_request{
  public:

    boost::beast::http::verb method()
    {
      return method_;
    }

    bool keep_alive(){
      return keep_alive_;
    }

    unsigned int version(){
      return version_;
    }

    std::string_view target_path(){
      return target_path_;
    }

    std::unordered_map<std::string, std::string>& route_params(){
      return route_params_;
    }

    template<class BodyType>
    explicit http_request(boost::beast::http::request<BodyType>& request)
    {
      std::string target_raw = request.target();
      std::string string_params;

      auto splitor = target_raw.find_first_of('?');
      if(splitor == std::string::npos)
        target_path_ = target_raw;
      else{
        target_path_ = target_raw.substr(0, splitor);
        string_params = target_raw.substr(splitor+1, target_raw.size()-1);
      }
      keep_alive_ = request.keep_alive();
      method_ = request.method();
      version_ = request.version();

    }
  protected:
    bool keep_alive_ = false;
    boost::beast::http::verb method_;
    std::string target_path_;
    http_header headers_;
    unsigned int version_;

    std::unordered_map<std::string, std::string> route_params_;
    friend class route_item;
  };
}


#endif //OBELISK_HTTP_REQUEST_H
