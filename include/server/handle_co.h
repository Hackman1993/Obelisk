//
// Created by Hackman.Lo on 4/25/2023.
//

#ifndef LIBRARYORGANIZED_HANDLE_IO_H
#define LIBRARYORGANIZED_HANDLE_IO_H
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include "common/request/http_request_raw.h"
namespace obelisk {

  class http_server;
  boost::asio::awaitable<bool> multipart_body_co_(http_server& server, boost::asio::ip::tcp::socket& socket, http_request_raw& request);
  boost::asio::awaitable<void> handle_co_(http_server& server, boost::asio::ip::tcp::socket socket);

  void request_header_handler_(http_server& server, boost::asio::ip::tcp::socket& socket, boost::asio::streambuf buffer, http_request_raw request);

} // obelisk

#endif //LIBRARYORGANIZED_HANDLE_IO_H
