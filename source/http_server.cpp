//
// Created by Hackman.Lo on 3/9/2023.
//

#include "http_server.h"

#include <fstream>
#include <filesystem>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include "exception/network_exception.h"
#include "exception/system_exception.h"
#include "session/plain_http_session.h"

using namespace boost::asio::ip;
namespace obelisk {


  http_server::http_server(const std::string &address, unsigned short port) {
    listen(address, port);
  }

  void http_server::listen(const std::string &address, unsigned short port) {
    try {
      std::unique_ptr<tcp::acceptor, void (*)(tcp::acceptor *)> pacceptor_(new tcp::acceptor(make_strand(ios_)), [](tcp::acceptor *pointer) {
        pointer->close();
        delete pointer;
      });
      tcp::endpoint endpoint{make_address(address), port};
      pacceptor_->open(endpoint.protocol());
      pacceptor_->set_option(boost::asio::socket_base::reuse_address(true));
      pacceptor_->bind(endpoint);
      pacceptor_->listen(boost::asio::socket_base::max_listen_connections);
      acceptors_.push_back(std::move(pacceptor_));
    } catch (boost::system::error_code &err) {
      throw exception::network_exception{err.what()};
    }
  }

  void http_server::run(unsigned int threads) {
    for (auto &acceptor_: acceptors_) {
      accept_(acceptor_.get());
    }

    boost::asio::signal_set signals(ios_, SIGINT, SIGTERM);
    signals.async_wait([&](boost::system::error_code const &, int) { ios_.stop(); });
    threads_.reserve(threads - 1);
    for (auto i = 0; i < threads - 1; ++i)
      threads_.emplace_back([this] { run_(); });

    run_();
    for (auto &t: threads_) t.join();
  }

  void http_server::run_() {
    try{
      ios_.run();
    } catch (std::exception& e){
      std::cout << e.what() << std::endl;
    }catch (exception_base& e){
      std::cout << e.what() << std::endl;
    }
  }

  void http_server::accept_(boost::asio::ip::tcp::acceptor* acceptor) {
    acceptor->async_accept(boost::asio::make_strand(ios_), std::bind(&http_server::on_accept_, this, acceptor, std::placeholders::_1, std::placeholders::_2));
  }

  void http_server::on_accept_(boost::asio::ip::tcp::acceptor* acceptor, boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
    if(ec)
    {
      std::cout << "Accept: " << ec.what() << std::endl;
    }
    else
    {
      std::make_shared<plain_http_session>(std::move(socket), *this)->run();
    }
    accept_(acceptor);
  }

}
