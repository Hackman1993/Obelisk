//
// Created by Hackman.Lo on 3/9/2023.
//

#ifndef OBELISK_HTTP_SERVER_H
#define OBELISK_HTTP_SERVER_H


#include <string>
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <sahara/string/string.h>

#include <boost/asio/awaitable.hpp>
#include "../common/request/http_request_raw.h"
#include "../middleware/middleware_trigger.h"
#include <sahara/time/time_duration.h>
#include "../common/route/http_router.h"
//#include "middleware/auth_middleware.h"
//#include "middleware/middleware_trigger.h"
using namespace std::chrono_literals;
namespace obelisk {
    class http_session;

    class http_server : public std::enable_shared_from_this<http_server> {
    public:
        http_server(const sahara::string &address, unsigned short port, int threads = std::thread::hardware_concurrency() + 2);

        void listen(const sahara::string &address, unsigned short port);

        void run();

        sahara::time::time_duration &recv_timeout();

        http_router &router() { return router_; }

        route_item &route(const sahara::string &route, const std::function<std::shared_ptr<http_response>(http_request &)> &handle) {
            return router_.add_router(route, handle);
        }


        std::vector<middleware_trigger> &middlewares() {
            return middlewares_;
        }

    public:
        std::uint32_t header_max_length() const { return header_max_length_; }
        void set_header_max_length(std::uint32_t header_max_length) { header_max_length_ = header_max_length; }
        void release_connection(const std::string &connection_key);

    protected:
        void listen_handler_(const std::shared_ptr<boost::asio::ip::tcp::acceptor> &acceptor, boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

        std::uint32_t thread_count_ = 0;
        http_router router_;    // Http Router
        boost::asio::io_service ios_;   // IO Service
        std::vector<std::thread> threads_;  // Thread Pool
        std::uint32_t header_max_length_ = 1024;
        std::vector<middleware_trigger> middlewares_;   // Middlewares
        sahara::time::time_duration recv_timeout_ = 60s;
        std::vector<std::shared_ptr<boost::asio::ip::tcp::acceptor>> acceptors_;    // 1 Service Can Listen Many Ports
        std::unordered_map<std::string, std::shared_ptr<http_session>> sessions_;   // Session Pool
    };
}


#endif //OBELISK_HTTP_SERVER_H
