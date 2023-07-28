//
// Created by Hackman.Lo on 7/6/2023.
//

#ifndef LIBRARYORGANIZED_HTTP_REQUEST_IO_H
#define LIBRARYORGANIZED_HTTP_REQUEST_IO_H
#include <string>
#include <vector>
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>

namespace obelisk {
    struct http_request_header_begin_io {
        std::string method_;
        std::string target_;
        std::string protocol_;
    };

    struct http_pair_data_io {
        std::string key_;
        std::string value_;
    };

    struct http_package_header_io {
        http_request_header_begin_io mat_;
        std::vector<http_pair_data_io> headers_;
    };

    class http_request_io {
    public:
        http_request_io(boost::asio::ip::tcp::socket& socket);
    protected:
        http_package_header_io header_;
        boost::asio::ip::tcp::socket &socket_;

    };

} // obelisk

#endif //LIBRARYORGANIZED_HTTP_REQUEST_IO_H
