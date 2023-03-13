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
    Derived &derived() {
      return static_cast<Derived &>(*this);
    }

    static constexpr std::size_t queue_limit = 8; // max responses
    std::vector<boost::beast::http::message_generator> response_queue_;

    boost::optional<boost::beast::http::request_parser<boost::beast::http::string_body>> parser_;

  protected:
    boost::beast::flat_buffer buffer_;

  public:
    // Construct the session
    http_session_base(boost::beast::flat_buffer buffer): buffer_(std::move(buffer)) {}



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

        // Create a websocket session, transferring ownership
        // of both the socket and the HTTP request.
        // TODO: Uncomment below

        return make_websocket_session(derived().release_stream(), parser_->release());
      }

      // Send the response
      // TODO: Uncomment Follows
      queue_write(handle_request(parser_->release()));

      // If we aren't at the queue limit, try to pipeline another request
      if (response_queue_.size() < queue_limit) do_read();
    }

    void queue_write(boost::beast::http::message_generator response) {
      // Allocate and store the work
      response_queue_.push_back(std::move(response));

      // If there was no previous work, start the write
      // loop
      if (response_queue_.size() == 1)
        do_write();
    }

    // Called to start/continue the write-loop. Should not be called when
    // write_loop is already active.
    //
    // Returns `true` if the caller may initiate a new read
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
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return derived().do_eof();
      }

      // Inform the queue that a write completed
      if (do_write()) {
        // Read another request
        do_read();
      }
    }

    template<class BodyType>
    boost::beast::http::message_generator handle_request(boost::beast::http::request<BodyType>&& req)
    {
      boost::beast::http::request<boost::beast::http::string_body> request;
      for(auto &data: req)
      {
        std::cout << data.name_string() << ":" << data.value() << std::endl;
      }
      std::cout<< req.at("user-agent") << std::endl;
      std::cout << req.body() << std::endl;
      std::cout<< req.target() << std::endl;
      std::cout << req.body() << std::endl;

      // Returns a bad request response
      auto const bad_request =
          [&req](boost::beast::string_view why)
          {
            boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::bad_request, req.version()};
            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = std::string(why);
            res.prepare_payload();
            return res;
          };

      // Returns a not found response
      auto const not_found =
          [&req](boost::beast::string_view target)
          {
            boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found, req.version()};
            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "The resource '" + std::string(target) + "' was not found.";
            res.prepare_payload();
            return res;
          };

      // Returns a server error response
      auto const server_error =
          [&req](boost::beast::string_view what)
          {
            boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::internal_server_error, req.version()};
            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "An error occurred: '" + std::string(what) + "'";
            res.prepare_payload();
            return res;
          };

      // Make sure we can handle the method
      if( req.method() != boost::beast::http::verb::get &&
          req.method() != boost::beast::http::verb::head)
        return bad_request("Unknown HTTP-method");

      // Request path must be absolute and not contain "..".
      if( req.target().empty() ||
          req.target()[0] != '/' ||
          req.target().find("..") != boost::beast::string_view::npos)
        return bad_request("Illegal request-target");

      // Build the path to the requested file
//      std::string path = path_cat(doc_root, req.target());
//      if(req.target().back() == '/')
//        path.append("index.html");

      // Attempt to open the file
      boost::beast::error_code ec;
      boost::beast::http::file_body::value_type body;
//      body.open(path.c_str(), boost::beast::file_mode::scan, ec);

      // Handle the case where the file doesn't exist
      if(ec == boost::beast::errc::no_such_file_or_directory)
        return not_found(req.target());

      // Handle an unknown error
      if(ec)
        return server_error(ec.message());

      // Cache the size since we need it after the move
      auto const size = body.size();

      // Respond to HEAD request
      if(req.method() == boost::beast::http::verb::head)
      {
        boost::beast::http::response<boost::beast::http::empty_body> res{boost::beast::http::status::ok, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        //res.set(boost::beast::http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
      }

      return not_found(req.target());
      // Respond to GET request
//      boost::beast::http::response<boost::beast::http::file_body> res{
//          std::piecewise_construct,
//          std::make_tuple(std::move(body)),
//          std::make_tuple(boost::beast::http::status::ok, req.version())};
//      res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
//      // res.set(boost::beast::http::field::content_type, mime_type(path));
//      res.content_length(size);
//      res.keep_alive(req.keep_alive());
    }
  };
}
#endif