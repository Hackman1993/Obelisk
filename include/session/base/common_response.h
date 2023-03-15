//
// Created by Hackman.Lo on 3/15/2023.
//

#ifndef OBELISK_COMMON_RESPONSE_H
#define OBELISK_COMMON_RESPONSE_H

#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/message_generator.hpp>

namespace obelisk{
  boost::beast::http::message_generator string_response(unsigned int status_code, boost::beast::string_view why, unsigned int version, bool keep_alive){
    boost::beast::http::response<boost::beast::http::string_body> res{(boost::beast::http::status)status_code, version};
    res.set(boost::beast::http::field::server, "Obelisk/1.0.0");
    res.set(boost::beast::http::field::content_type, "text/html");
    res.keep_alive(keep_alive);
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
  }
}

#endif //OBELISK_COMMON_RESPONSE_H
