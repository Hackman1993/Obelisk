//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_PLAIN_HTTP_SESSION_H
#define OBELISK_PLAIN_HTTP_SESSION_H

#include <boost/asio/ssl/context.hpp>
#include "session/base/http_session_base.h"

namespace obelisk{
  class plain_http_session: public http_session_base<plain_http_session>, public std::enable_shared_from_this<plain_http_session>
  {
    boost::beast::tcp_stream stream_;
  public:
    // Create the session
    plain_http_session(boost::beast::tcp_stream&& stream, http_server& server, boost::beast::flat_buffer&& buffer):
        http_session_base<plain_http_session>(std::move(buffer), server), stream_(std::move(stream))
    {
    }

    // Start the session
    void run()
    {
      this->do_read();
    }

    // Called by the base class
    boost::beast::tcp_stream& stream() { return stream_; }

    // Called by the base class
    boost::beast::tcp_stream release_stream() { return std::move(stream_); }

    // Called by the base class
    void do_eof() {
      // Send a TCP shutdown
      boost::beast::error_code ec;
      // TODO: Which Is Better
      stream_.socket().close();
      // stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    }
  };
}


#endif //OBELISK_PLAIN_HTTP_SESSION_H
