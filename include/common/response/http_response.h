//
// Created by Hackman.Lo on 3/15/2023.
//

#ifndef OBELISK_HTTP_RESPONSE_H
#define OBELISK_HTTP_RESPONSE_H
#include <boost/beast/http/message_generator.hpp>
namespace obelisk {

  class http_response {
  public:
    http_response(unsigned int code, bool keep_alive): resp_code_(code), keep_alive_(keep_alive){};

    virtual operator boost::beast::http::message_generator() = 0;
    virtual ~http_response() {};
  protected:
    unsigned int resp_code_ = 200;
    bool keep_alive_ = false;
  };

} // obelisk

#endif //OBELISK_HTTP_RESPONSE_H
