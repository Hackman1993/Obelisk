//
// Created by Hackman.Lo on 3/9/2023.
//

#ifndef OBELISK_HTTP_SERVER_H
#define OBELISK_HTTP_SERVER_H


#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "route/http_router.h"

namespace obelisk{
  class http_server : public std::enable_shared_from_this<http_server> {
  public:
    http_server(const std::string& address, unsigned short port);
    void listen(const std::string& address, unsigned short port);

    void run(unsigned int threads);
    http_router& router(){
      return router_;
    }

    route_item& route(std::string_view route, std::function<std::unique_ptr<http_response>(http_request&)> handle){
      return router_.add_router(route,handle);
    }

  private:
    void run_();
    void accept_(boost::asio::ip::tcp::acceptor* acceptor);
    void on_accept_(boost::asio::ip::tcp::acceptor* acceptor,  boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

    boost::asio::io_service ios_;
    std::vector<std::thread> threads_;
    http_router router_;
    std::vector<std::unique_ptr<boost::asio::ip::tcp::acceptor, void(*)(boost::asio::ip::tcp::acceptor*)>> acceptors_;
  };
}


#endif //OBELISK_HTTP_SERVER_H
