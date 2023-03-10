//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_SESSION_DETECTOR_H
#define OBELISK_SESSION_DETECTOR_H
#include <memory>
#include <boost/beast.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/dispatch.hpp>
#include "plain_http_session.h"
#include "ssl_http_session.h"

namespace obelisk{
  class session_detector : public std::enable_shared_from_this<session_detector>
  {
    boost::beast::tcp_stream stream_;
    boost::asio::ssl::context& ctx_;
    boost::beast::flat_buffer buffer_;

  public:
    explicit
    session_detector(boost::asio::ip::tcp::socket&& socket,boost::asio::ssl::context& ctx): stream_(std::move(socket)), ctx_(ctx)
    {
    }

    // Launch the detector
    void run()
    {
      boost::asio::dispatch(stream_.get_executor(), boost::beast::bind_front_handler(&session_detector::on_run, this->shared_from_this()));
    }

    void on_run()
    {
      stream_.expires_after(std::chrono::seconds(30));
      boost::beast::async_detect_ssl(stream_, buffer_, boost::beast::bind_front_handler(&session_detector::on_detect, this->shared_from_this()));
    }

    void on_detect(boost::beast::error_code ec, bool result) {
      if(ec) return;

      if(result)
      {
        // Launch SSL session
        std::make_shared<ssl_http_session>(std::move(stream_), ctx_, std::move(buffer_))->run();
        return;
      }

      // Launch plain session
      std::make_shared<plain_http_session>(std::move(stream_), std::move(buffer_))->run();
    }
  };
}

#endif //OBELISK_SESSION_DETECTOR_H
