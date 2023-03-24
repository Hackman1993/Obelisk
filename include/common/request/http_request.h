//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_HTTP_REQUEST_H
#define OBELISK_HTTP_REQUEST_H
#include <boost/beast.hpp>
#include "common/details/request_file.h"
#include "common/details/request_param_container.h"
#include "common/details/nocase_support.h"
#include "common/details/http_header.h"

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

    bool is_upgrade(){
      return is_upgrade_;
    }


    http_request() = default;
  protected:
    unsigned int version_;
    std::string target_path_;
    bool is_upgrade_ : 1 = false;
    bool keep_alive_ : 1 = false;
    http_header headers_;
    std::unordered_map<std::string, request_file> file_bag_;
    boost::beast::http::verb method_;

    request_param_container request_params_;
    std::unordered_map<std::string, std::string> route_params_;
    friend class route_item;
    friend class http_request_parser;
  };
}


#endif //OBELISK_HTTP_REQUEST_H
