//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_PLAIN_HTTP_SESSION_H
#define OBELISK_PLAIN_HTTP_SESSION_H

#include <boost/asio/ssl/context.hpp>
#include "common/session/base/http_session_base.h"
#include "common/session/base/common_response.h"
#include "common/request/http_request.h"
#include "common/request/http_request_parser.h"

namespace obelisk{
  class plain_http_session: public std::enable_shared_from_this<plain_http_session>
  {
    http_server& server_;
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    static constexpr std::size_t queue_limit = 8;
    std::vector<boost::beast::http::message_generator> response_queue_;
    boost::optional<http_request_parser> parser_;
  public:
    // Create the session
    plain_http_session(boost::asio::ip::tcp::socket&& socket, http_server& server): stream_(std::move(socket)), server_(server){
      boost::beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));
    }


    // Start the session
    void run()
    {
      this->do_read();
    }

    void do_read() {
      parser_.emplace();
      parser_->body_limit(1000*1024*1024);
      boost::beast::http::async_read(stream_, buffer_, *parser_, boost::beast::bind_front_handler(&plain_http_session::on_read, shared_from_this()));
    }

    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred) {
      boost::ignore_unused(bytes_transferred);

      if (ec) {
        //std::cout << ec.what() <<ec.value() << std::endl;
        return do_eof();
      }else{
        if (parser_->get().is_upgrade()) {
          boost::beast::get_lowest_layer(stream_).expires_never();
          //std::make_shared<plain_websocket_session>(std::move(stream_))->run(parser_->release());
        }
        queue_write(handle_request(std::move(parser_->release())));
        if (response_queue_.size() < queue_limit) do_read();
      }
    }

    void do_eof() {
      // TODO: Which Is Better
      stream_.socket().close();
//       boost::beast::error_code ec;
//       stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    }

    void queue_write(boost::beast::http::message_generator response) {
      response_queue_.push_back(std::move(response));
      if (response_queue_.size() == 1) do_write();
    }

    bool do_write() {
      bool const was_full = response_queue_.size() == queue_limit;

      if (!response_queue_.empty()) {
        boost::beast::http::message_generator msg = std::move(response_queue_.front());
        response_queue_.erase(response_queue_.begin());
        bool keep_alive = msg.keep_alive();
        boost::beast::async_write(stream_, std::move(msg), boost::beast::bind_front_handler(&plain_http_session::on_write, shared_from_this(), keep_alive));
      }

      return was_full;
    }

    void on_write(bool keep_alive, boost::beast::error_code ec, std::size_t bytes_transferred) {
      boost::ignore_unused(bytes_transferred);
      if(ec)
        throw exception::network_exception{ec.what()};

      if (!keep_alive) {
        return do_eof();
      }

      if (do_write()) {
        do_read();
      }
    }

    boost::beast::http::message_generator handle_request(http_request&& req)
    {
      http_request friendly_request(req);
      for(auto & item : server_.middlewares()){
        auto result = item.handle(friendly_request);
        if(result) return *result;
      }
      auto result = server_.router().handle(friendly_request);
      if(result) return *result;

      return string_response(404, "Not Found!", req.version(), req.keep_alive());
    }
  };
}


#endif //OBELISK_PLAIN_HTTP_SESSION_H
