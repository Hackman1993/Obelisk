//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_PLAIN_WEBSOCKET_SESSION_H
#define OBELISK_PLAIN_WEBSOCKET_SESSION_H
#include <memory>
#include "common/session/base/websocket_session_base.h"

namespace obelisk{
  class plain_websocket_session: public websocket_session_base<plain_websocket_session>, public std::enable_shared_from_this<plain_websocket_session>
  {
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
  public:
    // Create the session
    explicit
    plain_websocket_session(boost::beast::tcp_stream&& stream) : ws_(std::move(stream)) { }

    // Called by the base class
    boost::beast::websocket::stream<boost::beast::tcp_stream>& ws()
    {
      return ws_;
    }
  };
}


#endif //OBELISK_PLAIN_WEBSOCKET_SESSION_H
