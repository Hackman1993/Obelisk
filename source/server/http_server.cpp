//
// Created by Hackman.Lo on 3/9/2023.
//

#include "server/http_server.h"

#include <fstream>
#include <boost/bind.hpp>
//#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>
#include <iostream>
#include "exception/network_exception.h"
#include "exception/system_exception.h"
#include "common/request/http_request_raw.h"
#include "utils/uuid.h"
//#include <boost/asio/detached.hpp>
#include <filesystem>
#include "common/session/http_session.h"
#include <boost/spirit/home/x3.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/algorithm/string_regex.hpp>
#include "common/request/request_header_parser.h"
using namespace boost::asio::ip;
namespace obelisk {
  struct body_meta{
    std::string name_;
    bool is_file_;
    std::string file_name_ = "./temp/" + boost::uuids::to_string(boost::uuids::random_generator()());
  };


  http_server::http_server(const sahara::string &address, unsigned short port, unsigned int threads): ios_(threads) {
    listen(address, port);
  }

  void http_server::listen(const sahara::string &address, unsigned short port) {
    try {
      std::shared_ptr<tcp::acceptor> pacceptor_ = std::make_shared<tcp::acceptor>(ios_);
      tcp::endpoint endpoint{make_address(address.to_std()), port};
      pacceptor_->open(endpoint.protocol());
      pacceptor_->set_option(boost::asio::socket_base::reuse_address(true));
      pacceptor_->bind(endpoint);
      pacceptor_->listen(boost::asio::socket_base::max_listen_connections);
      pacceptor_->async_accept([this, pacceptor_](boost::system::error_code ec, tcp::socket socket) {
        this->listen_handler_(pacceptor_, ec, std::move(socket));
      });
      acceptors_.emplace_back(pacceptor_);

    } catch (boost::system::error_code &err) {
      throw exception::network_exception{err.what()};
    }
  }

  void http_server::run(unsigned int threads) {

    boost::asio::signal_set signals(ios_, SIGINT, SIGTERM);
    signals.async_wait([&](boost::system::error_code const &, int) { ios_.stop(); });
    threads_.reserve(threads - 1);
    for (auto i = 0u; i < threads - 1; ++i)
      threads_.emplace_back([this] { run_(); });

    run_();
    for (auto &t: threads_) t.join();
  }

  void http_server::run_() {
    try{
      ios_.run();
    } catch (exception_base& e){
      std::cout << e.message().to_std() << std::endl;
    } catch (std::exception& e){
      std::cout << e.what() << std::endl;
    }
  }

  void http_server::listen_handler_(std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor, boost::system::error_code ec, boost::asio::ip::tcp::socket socket){
    if(ec) {
      socket.close();
      return;
    }


    std::string uuid = sahara::utils::uuid::generate();
    std::shared_ptr<http_session> session = std::make_shared<http_session>(uuid, socket, *this);
    sessions_.emplace(std::pair<std::string, std::shared_ptr<http_session>>(uuid, session));
    session->run();
    acceptor->async_accept([this, acceptor](boost::system::error_code ec, tcp::socket socket) {
      this->listen_handler_(acceptor, ec, std::move(socket));
    });
  }

void http_server::release_connection(const std::string &connection_key) {
    sessions_.erase(connection_key);
  }

  sahara::time::time_duration &http_server::recv_timeout() {
    return recv_timeout_;
  }
}
BOOST_FUSION_ADAPT_STRUCT(obelisk::body_meta, name_, is_file_)