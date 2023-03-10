//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_SSL_WEBSOCKET_SESSION_H
#define OBELISK_SSL_WEBSOCKET_SESSION_H

#include <boost/beast/ssl/ssl_stream.hpp>
#include "session/base/websocket_session_base.h"
namespace obelisk{
  class ssl_websocket_session: public websocket_session_base<ssl_websocket_session>, public std::enable_shared_from_this<ssl_websocket_session>
  {
    boost::beast::websocket::stream<
    boost::beast::ssl_stream<boost::beast::tcp_stream>> ws_;

  public:
    // Create the ssl_websocket_session
    explicit
    ssl_websocket_session(boost::beast::ssl_stream<boost::beast::tcp_stream>&& stream) : ws_(std::move(stream))
    {
    }

    // Called by the base class
    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>>& ws()
    {
      return ws_;
    }
  };
}




#endif //OBELISK_SSL_WEBSOCKET_SESSION_H
