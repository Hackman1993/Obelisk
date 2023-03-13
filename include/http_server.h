//
// Created by Hackman.Lo on 3/9/2023.
//

#ifndef OBELISK_HTTP_SERVER_H
#define OBELISK_HTTP_SERVER_H


#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace obelisk{
  class http_server : public std::enable_shared_from_this<http_server> {
  public:
    http_server(const std::string& address, unsigned short port);
    void listen(const std::string& address, unsigned short port);

    void run(unsigned int threads);

    void load_certificate(std::string certificate_path, std::string private_key_path);
  private:
    void run_();
    void accept_(boost::asio::ip::tcp::acceptor* acceptor);
    void on_accept_(boost::asio::ip::tcp::acceptor* acceptor,  boost::system::error_code ec, boost::asio::ip::tcp::socket socket);



    boost::asio::io_service ios_;
    std::vector<std::thread> threads_;
    boost::asio::ssl::context ssl_context_{boost::asio::ssl::context::tlsv12};
    std::vector<std::unique_ptr<boost::asio::ip::tcp::acceptor, void(*)(boost::asio::ip::tcp::acceptor*)>> acceptors_;
  };
}


#endif //OBELISK_HTTP_SERVER_H
