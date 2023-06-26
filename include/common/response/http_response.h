//
// Created by Hackman.Lo on 3/15/2023.
//

#ifndef OBELISK_HTTP_RESPONSE_H
#define OBELISK_HTTP_RESPONSE_H

#include <boost/asio/ip/tcp.hpp>
#include "common/details/http_header.h"
namespace obelisk {
    class http_session;

    class http_response {
    public:

        http_response(unsigned int code) : resp_code_(code) {};

        virtual ~http_response() {};
        std::string serialize_header();
        virtual std::size_t write_content(std::shared_ptr<http_session> session) final;
    protected:
        unsigned int resp_code_ = 200;
        http_header headers_;
        virtual std::size_t write_content_(boost::asio::ip::tcp::socket& socket);
    };

} // obelisk

#endif //OBELISK_HTTP_RESPONSE_H
