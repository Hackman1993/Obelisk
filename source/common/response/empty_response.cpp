//
// Created by Hackman.Lo on 7/26/2023.
//

#include "common/response/empty_response.h"

namespace obelisk {
    size_t empty_response::write_content_(boost::asio::ip::tcp::socket &socket) {
        headers_.emplace("Connection", "close");
        headers_.emplace("Content-Type", "text/html; charset=UTF-8");
        std::string http_header_str = serialize_header();
        return socket.write_some(boost::asio::buffer(http_header_str));
    }
} // obelisk