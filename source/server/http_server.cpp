/*
 * @File: http_server.cpp
 * @Author: Hackman.Lo
 * @Email: admin@yytec.net
 * @Date: 2023-06-20
 * @Description: Implementation of http_server
 */
#include <iostream>
#include <filesystem>
#include <sahara/utils/uuid.h>
#include "server/http_server.h"
#include "common/session/http_session.h"
#include "exception/network_exception.h"
#include "common/request/http_request_raw.h"

using namespace boost::asio::ip;
namespace obelisk {

    http_server::http_server(const sahara::string &address, unsigned short port, int threads) : ios_(threads) {
        listen(address, port);
    }

    /**
     * @class http_server
     * @function listen
     * @param address Listen Address
     * @param port Listen Port
     * @description Listen to the specified address and port, set socket reuse
     */
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
        // Bind Can only be called here
        boost::asio::signal_set signals(ios_, SIGINT, SIGTERM);
        signals.async_wait([&](boost::system::error_code const &, int) { ios_.stop(); });

        threads_.reserve(threads - 1);
        for (auto i = 0u; i < threads - 1; ++i)
            threads_.emplace_back([this] { ios_.run(); });
        // Run last thread in main thread
        for (auto &t: threads_) t.join();
    }

    void http_server::listen_handler_(const std::shared_ptr<boost::asio::ip::tcp::acceptor>& acceptor, boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (ec) {
            socket.close();
            return;
        }
        std::string uuid = sahara::utils::uuid::generate();
        std::shared_ptr<http_session> session = std::make_shared<http_session>(uuid, socket, *this);
        sessions_.emplace(std::pair<std::string, std::shared_ptr<http_session>>(uuid, session));
        acceptor->async_accept([this, acceptor](boost::system::error_code ec, tcp::socket socket) {
            this->listen_handler_(acceptor, ec, std::move(socket));
        });
    }

    void http_server::release_connection(const std::string &connection_key) {
        if(sessions_.contains(connection_key))
            sessions_.erase(connection_key);
    }

    sahara::time::time_duration &http_server::recv_timeout() {
        return recv_timeout_;
    }
}