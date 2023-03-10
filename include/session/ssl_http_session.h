//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_SSL_HTTP_SESSION_H
#define OBELISK_SSL_HTTP_SESSION_H

#include <boost/beast/ssl/ssl_stream.hpp>
#include "session/base/http_session_base.h"

namespace obelisk {
  class ssl_http_session: public http_session_base<ssl_http_session>, public std::enable_shared_from_this<ssl_http_session>
  {
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;

  public:
    // Create the http_session
    ssl_http_session(boost::beast::tcp_stream&& stream, boost::asio::ssl::context& ctx, boost::beast::flat_buffer&& buffer): http_session_base<ssl_http_session>(std::move(buffer)), stream_(std::move(stream), ctx)
    {
    }

    // Start the session
    void run()
    {
      // Set the timeout.
      boost::beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

      // Perform the SSL handshake
      // Note, this is the buffered version of the handshake.
      stream_.async_handshake(boost::asio::ssl::stream_base::server, buffer_.data(), boost::beast::bind_front_handler(&ssl_http_session::on_handshake, shared_from_this()));
    }

    // Called by the base class
    boost::beast::ssl_stream<boost::beast::tcp_stream>& stream()
    {
      return stream_;
    }

    // Called by the base class
    boost::beast::ssl_stream<boost::beast::tcp_stream> release_stream()
    {
      return std::move(stream_);
    }

    // Called by the base class
    void do_eof()
    {
      // Set the timeout.
      boost::beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

      // Perform the SSL shutdown
      stream_.async_shutdown(boost::beast::bind_front_handler(&ssl_http_session::on_shutdown, shared_from_this()));
    }

  private:
    void on_handshake(boost::beast::error_code ec, std::size_t bytes_used)
    {
      if(ec)
      {
        std::cout << "SSL Handshake Error: " << ec.what() << std::endl;
        return;
      }


      // Consume the portion of the buffer used by the handshake
      buffer_.consume(bytes_used);

      do_read();
    }

    void on_shutdown(boost::beast::error_code ec)
    {
      if(ec)
      {
        std::cout << "SSL Shutdown Error:" << ec.what() << std::endl;
        return;
      }

      // At this point the connection is closed gracefully
    }
  };
}


#endif //OBELISK_SSL_HTTP_SESSION_H
