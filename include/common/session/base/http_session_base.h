#ifndef OBELISK_HTTP_SESSION_BASE_H
#define OBELISK_HTTP_SESSION_BASE_H
#include <memory>
#include <iostream>
#include <boost/beast.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include "exception/network_exception.h"
#include "common/session/plain_websocket_session.h"

#include "http_server.h"
#include <boost/url.hpp>
#include <boost/algorithm/string.hpp>
namespace obelisk {
//  template<class Body, class Allocator>
//  void make_websocket_session(boost::beast::tcp_stream stream, boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
//  {
//    std::make_shared<plain_websocket_session>(std::move(stream))->run(std::move(req));
//  }
//
//  template<class Body, class Allocator>
//  void make_websocket_session(boost::beast::ssl_stream<boost::beast::tcp_stream> stream, boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
//  {
//    std::make_shared<ssl_websocket_session>(
//        std::move(stream))->run(std::move(req));
//  }

}
#endif