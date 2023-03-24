//
// Created by Hackman.Lo on 3/16/2023.
//

#ifndef OBELISK_EMPTY_RESPONSE_H
#define OBELISK_EMPTY_RESPONSE_H

#include <string>
#include <boost/beast/http/string_body.hpp>
#include <utility>
#include <boost/beast/http/empty_body.hpp>
#include "http_response.h"

namespace obelisk {

  class empty_response : public http_response {
  public:
    empty_response(unsigned int code = 200): http_response(code){}
    ~empty_response(){};
    operator boost::beast::http::message_generator() override {
      boost::beast::http::response<boost::beast::http::empty_body> res{(boost::beast::http::status)resp_code_, 11};
      res.set(boost::beast::http::field::server, "Obelisk/1.0.0");
      res.set(boost::beast::http::field::content_type, "text/html");
      res.keep_alive(keep_alive_);
      return res;
    }

  protected:
    std::string content_;
  };

} // obelisk

#endif //OBELISK_EMPTY_RESPONSE_H
