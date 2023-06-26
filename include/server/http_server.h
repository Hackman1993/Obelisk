//
// Created by Hackman.Lo on 3/9/2023.
//

#ifndef OBELISK_HTTP_SERVER_H
#define OBELISK_HTTP_SERVER_H


#include <string>
#include <boost/asio.hpp>
#include <string/string.h>

#include <boost/asio/awaitable.hpp>
#include "common/request/http_request_raw.h"
#include "middleware/middleware_trigger.h"
#include "middleware/auth_middleware.h"
#include <time/time_duration.h>
#include "common/route/http_router.h"
//#include "middleware/auth_middleware.h"
//#include "middleware/middleware_trigger.h"
using namespace std::chrono_literals;
namespace obelisk {
  class http_session;
  class http_server : public std::enable_shared_from_this<http_server> {
  public:
    http_server(const sahara::string &address, unsigned short port, unsigned int threads = std::thread::hardware_concurrency() + 2);

    void listen(const sahara::string &address, unsigned short port);

    void run(unsigned int threads);

    sahara::time::time_duration& recv_timeout();

        http_router &router() {
            return router_;
        }

        route_item &route(const sahara::string &route,
                          const std::function<std::unique_ptr<http_response>(http_request &)> &handle) {
            return router_.add_router(route, handle);
        }


    virtual void add_default_middlewares() {
      middlewares_.emplace_back("token_auth", &auth_middleware::handle);
    }

    std::vector<middleware_trigger> &middlewares() {
      return middlewares_;
    }

  public:
    std::uint32_t header_max_length() const { return header_max_length_; }
    void set_header_max_length(std::uint32_t header_max_length) { header_max_length_ = header_max_length; }
    std::uint32_t body_block_length() const { return body_block_length_; }
    void set_body_block_length(std::uint32_t body_block_length) { body_block_length_ = body_block_length; }

    void release_connection(const std::string& connection_key);
  protected:
    void run_();
    void listen_handler_(std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor, boost::system::error_code ec, boost::asio::ip::tcp::socket socket);
    boost::asio::io_service ios_;
    std::vector<std::thread> threads_;
    std::uint32_t header_max_length_ = 1024;
    std::uint32_t body_block_length_ = 1024000;


    sahara::time::time_duration recv_timeout_ = 30s;
    std::vector<middleware_trigger> middlewares_;

//        void accept_(boost::asio::ip::tcp::acceptor *acceptor);
//
//        void on_accept_(boost::asio::ip::tcp::acceptor *acceptor, boost::system::error_code ec,
//                        boost::asio::ip::tcp::socket socket);
//
        http_router router_;


    std::vector<std::shared_ptr<boost::asio::ip::tcp::acceptor>> acceptors_;
    std::unordered_map<std::string, std::shared_ptr<http_session>> sessions_;
  };
}


#endif //OBELISK_HTTP_SERVER_H
