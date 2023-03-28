//
// Created by Hackman.Lo on 3/16/2023.
//

#ifndef OBELISK_STRING_RESPONSE_H
#define OBELISK_STRING_RESPONSE_H

#include <string>
#include <boost/beast/http/message_generator.hpp>
#include <boost/beast/http/string_body.hpp>
#include <utility>
#include "http_response.h"

namespace obelisk {

  class string_response : public http_response {
  public:
    string_response(unsigned int code, std::string content, bool keep_alive): http_response(code, keep_alive), content_(std::move(content)){}
    ~string_response(){};
    operator boost::beast::http::message_generator() override {
      boost::beast::http::response<boost::beast::http::string_body> res{(boost::beast::http::status)resp_code_, 11};
      res.set(boost::beast::http::field::server, "Obelisk/1.0.0");
      res.set(boost::beast::http::field::content_type, "text/html");
      res.keep_alive(keep_alive_);
      res.body() = content_;
      res.prepare_payload();
      return res;
    }

  protected:
    std::string content_;
  };

} // obelisk

#endif //OBELISK_STRING_RESPONSE_H
