//
// Created by Hackman.Lo on 4/26/2023.
//

#include "common/request/request_header_parser.h"
#include <boost/spirit/home/x3.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

#define BOOST_SPIRIT_X3_DEBUG
namespace obelisk {
  bool parse_request_header(std::string_view source, http_request_raw &request) {
    using namespace boost::spirit::x3;
    auto method_parser = (+~char_(" \r\n") >> " " >> +~char_(" \r\n") >> " " >> no_case["HTTP/"] >> +~char_(" \r\n") >> "\r\n")[([&](auto &ctx) {
      auto &data = _attr(ctx);
      request.method_ = boost::fusion::at_c<0>(data);
      request.path_ = boost::fusion::at_c<1>(data);
      request.version_ = boost::fusion::at_c<2>(data);
      _pass(ctx) = true;
    })];

    auto keep_alive_parser = (no_case["Connection"] >> ": " >> (+~char_("\r\n")) >> "\r\n")[([&](auto &ctx) {
      request.keep_alive_ = boost::algorithm::iequals(_attr(ctx), "keep-alive");
      request.headers_.emplace("Connection", _attr(ctx));
      _pass(ctx) = true;
    })];

    auto boundary_parser = (no_case["Content-Type"] >> ": " >> no_case["multipart/form-data; boundary="] >> (+~char_("\r\n")) >> "\r\n")[([&](auto &ctx) {
      request.boundary_ = _attr(ctx);
      request.headers_.emplace("Content-Type", "multipart/form-data; boundary="+_attr(ctx));
      _pass(ctx) = true;
    })];

    auto unhandled_item_parser = (+~char_(":\r\n") >> ": " >> (+~char_("\r\n")) >> "\r\n")[([&](auto &ctx) {
      auto &data = _attr(ctx);
      auto &data1 = boost::fusion::at_c<0>(data);
      auto &data2 = boost::fusion::at_c<1>(data);
      request.headers_.emplace(boost::fusion::at_c<0>(data), boost::fusion::at_c<1>(data));
      _pass(ctx) = true;
    })];

    auto content_length = (no_case["Content-Length"] >> ": " >> ulong_long >> "\r\n")[([&](auto &ctx) {
      request.content_length_ = _attr(ctx);
      //TODO: ulonglong conversion
      //request.headers_.emplace("Content-Length", sahara::string::from_ulonglong(_attr(ctx)));
      _pass(ctx) = true;
    })];

    auto header_single_line_parser = (content_length | keep_alive_parser | boundary_parser | unhandled_item_parser);

    auto header_final_parser = method_parser >> +header_single_line_parser >> "\r\n";
     auto result = parse(source.begin(), source.end(), header_final_parser);
    if (!result) {
      throw std::runtime_error("parse error");
    }
    return true;
  }
} // obelisk