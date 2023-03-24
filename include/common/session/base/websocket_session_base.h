//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_WEBSOCKET_SESSION_BASE_H
#define OBELISK_WEBSOCKET_SESSION_BASE_H

#include <boost/beast.hpp>
#include "exception/network_exception.h"

namespace obelisk{
  template<class Derived>
  class websocket_session_base
  {
    // Access the derived class, this is part of
    // the Curiously Recurring Template Pattern idiom.
    Derived& derived() { return static_cast<Derived&>(*this); }

    boost::beast::flat_buffer buffer_;

    // Start the asynchronous operation
    template<class Body, class Allocator> void do_accept(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
    {
      // Set suggested timeout settings for the websocket
      derived().ws().set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

      // Set a decorator to change the Server of the handshake
      derived().ws().set_option(boost::beast::websocket::stream_base::decorator(
              [](boost::beast::websocket::response_type& res)
              {
                res.set(boost::beast::http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        " advanced-server-flex");
              }));

      // Accept the websocket handshake
      derived().ws().async_accept(req, boost::beast::bind_front_handler(&websocket_session_base::on_accept, derived().shared_from_this()));
    }

  private:
    void on_accept(boost::beast::error_code ec)
    {
      if(ec) throw exception::network_exception{ec.what()};
      // Read a message
      do_read();
    }

    void do_read()
    {
      // Read a message into our buffer
      derived().ws().async_read(buffer_, boost::beast::bind_front_handler(&websocket_session_base::on_read, derived().shared_from_this()));
    }

    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
    {
      boost::ignore_unused(bytes_transferred);

      // This indicates that the websocket_session was closed
      if(ec == boost::beast::websocket::error::closed)
        return;

      if(ec)
        throw exception::network_exception{ec.what()};

      // Echo the message
      derived().ws().text(derived().ws().got_text());
      derived().ws().async_write(buffer_.data(), boost::beast::bind_front_handler(&websocket_session_base::on_write, derived().shared_from_this()));
    }

    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
    {
      boost::ignore_unused(bytes_transferred);

      if(ec) throw exception::network_exception{ec.what()};

      // Clear the buffer
      buffer_.consume(buffer_.size());

      // Do another read
      do_read();
    }

  public:
    // Start the asynchronous operation
    template<class Body, class Allocator>
    void run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
    {
      // Accept the WebSocket upgrade request
      do_accept(std::move(req));
    }
  };
}


#endif //OBELISK_WEBSOCKET_SESSION_BASE_H
