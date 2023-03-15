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
      method_ = request.method_string();
    }
  protected:
    bool keep_alive_ = false;
    std::string method_;
    std::string target_path_;
    http_header headers_;
  };
}


#endif //OBELISK_HTTP_REQUEST_H
