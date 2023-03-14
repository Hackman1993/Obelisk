#ifndef OBELISK_HTTP_SESSION_BASE_H
#define OBELISK_HTTP_SESSION_BASE_H
#include <memory>
#include <iostream>
#include <boost/beast.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include "exception/network_exception.h"
#include "../plain_websocket_session.h"
#include "../ssl_websocket_session.h"

#include "../../http_server.h"
#include <boost/url.hpp>
#include <boost/algorithm/string.hpp>
namespace obelisk {
  template<class Body, class Allocator>
  void make_websocket_session(boost::beast::tcp_stream stream, boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
  {
    std::make_shared<plain_websocket_session>(std::move(stream))->run(std::move(req));
  }

  template<class Body, class Allocator>
  void make_websocket_session(boost::beast::ssl_stream<boost::beast::tcp_stream> stream, boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
  {
    std::make_shared<ssl_websocket_session>(
        std::move(stream))->run(std::move(req));
  }
  template<class Derived>
  class http_session_base {
  protected:
    Derived &derived() {
      return static_cast<Derived &>(*this);
    }

    http_server& server_;
    static constexpr std::size_t queue_limit = 8;
    std::vector<boost::beast::http::message_generator> response_queue_;
    boost::optional<boost::beast::http::request_parser<boost::beast::http::string_body>> parser_;

  protected:
    boost::beast::flat_buffer buffer_;

  public:
    http_session_base(boost::beast::flat_buffer buffer, http_server& server): buffer_(std::move(buffer)), server_(server) {}

    void do_read() {
      parser_.emplace();
      parser_->body_limit(10000);
      boost::beast::get_lowest_layer(derived().stream()).expires_after(std::chrono::seconds(30));
      boost::beast::http::async_read(derived().stream(), buffer_, *parser_, boost::beast::bind_front_handler(&http_session_base::on_read, derived().shared_from_this()));
    }

    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred) {
      boost::ignore_unused(bytes_transferred);

      if (ec)
        return derived().do_eof();

      if (boost::beast::websocket::is_upgrade(parser_->get())) {
        boost::beast::get_lowest_layer(derived().stream()).expires_never();

        return make_websocket_session(derived().release_stream(), parser_->release());
      }
      queue_write(handle_request(std::move(parser_->release())));

      if (response_queue_.size() < queue_limit) do_read();
    }

    void queue_write(boost::beast::http::message_generator response) {
      response_queue_.push_back(std::move(response));

      if (response_queue_.size() == 1)
        do_write();
    }

    bool do_write() {
      bool const was_full = response_queue_.size() == queue_limit;

      if (!response_queue_.empty()) {
        boost::beast::http::message_generator msg = std::move(response_queue_.front());
        response_queue_.erase(response_queue_.begin());

        bool keep_alive = msg.keep_alive();

        boost::beast::async_write(derived().stream(), std::move(msg), boost::beast::bind_front_handler(&http_session_base::on_write, derived().shared_from_this(), keep_alive));
      }

      return was_full;
    }

    void on_write(bool keep_alive, boost::beast::error_code ec, std::size_t bytes_transferred) {
      boost::ignore_unused(bytes_transferred);
      if(ec)
        throw exception::network_exception{ec.what()};

      if (!keep_alive) {
        return derived().do_eof();
      }

      if (do_write()) {
        do_read();
      }
    }

    template<class BodyType>
    boost::beast::http::message_generator pre_response(boost::beast::http::request<BodyType>& req){
      if( req.target().empty() || req.target()[0] != '/' || req.target().find("..") != boost::beast::string_view::npos)
        return string_response(400, "Illegal request-target", req);


      std::string target_raw = req.target();
      std::string target;
      std::string string_params;
      auto splitor = target_raw.find_first_of('?');
      if(splitor == std::string::npos)
        target = target_raw;
      else{
        target = target_raw.substr(0, splitor);
        string_params = target_raw.substr(splitor+1, target_raw.size()-1);
      }

      std::vector<std::string> split_string;

      boost::split(split_string, req.target(), boost::is_any_of("?"), boost::token_compress_on);


      std::string path = std::string("./webroot").append( req.target());
      if(req.target().back() == '/') path.append("index.html");

      boost::beast::error_code ec;
      boost::beast::http::file_body::value_type body;
      body.open(path.c_str(), boost::beast::file_mode::scan, ec);
      if(ec)
        return string_response(500, ec.message(), req);

      // Cache the size since we need it after the move
      auto const size = body.size();

      if(req.method() == boost::beast::http::verb::head)
      {
        boost::beast::http::response<boost::beast::http::empty_body> res{boost::beast::http::status::ok, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
      }

      boost::beast::http::response<boost::beast::http::file_body> res{ std::piecewise_construct, std::make_tuple(std::move(body)),
          std::make_tuple(boost::beast::http::status::ok, req.version())};
      res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
      // res.set(boost::beast::http::field::content_type, mime_type(path));
      res.content_length(size);
      res.keep_alive(req.keep_alive());
      return res;
    }

    template<class BodyType>
    boost::beast::http::message_generator string_response(unsigned int status_code, boost::beast::string_view why, boost::beast::http::request<BodyType>& req){
      boost::beast::http::response<boost::beast::http::string_body> res{(boost::beast::http::status)status_code, req.version()};
      res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
      res.set(boost::beast::http::field::content_type, "text/html");
      res.keep_alive(req.keep_alive());
      res.body() = std::string(why);
      res.prepare_payload();
      return res;
    }
    template<class BodyType>
    boost::beast::http::message_generator handle_request(boost::beast::http::request<BodyType> req)
    {
      for(auto &data: req)
      {
        std::cout << data.name_string() << ":" << data.value() << std::endl;
      }
      return pre_response(req);

    }
  };
}
#endif