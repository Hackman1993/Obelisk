//
// Created by Hackman.Lo on 6/25/2023.
//

#include "common/response/json_response.h"
#include <boost/asio.hpp>

namespace obelisk {
    json_response::json_response(boost::json::object object, std::size_t code) : http_response(code) {
        headers_["Content-Type"] = sahara::string("application/json");
        json_data_ = boost::json::serialize(object);
        headers_["Content-Length"] = std::to_string(json_data_.size());
    }

    json_response::json_response(boost::json::array object, std::size_t code) : http_response(code) {
        headers_["Content-Type"] = sahara::string("application/json");
        json_data_ = boost::json::serialize(object);
        headers_["Content-Length"] = std::to_string(json_data_.size());
    }

    size_t json_response::write_content_(boost::asio::ip::tcp::socket &socket) {
        headers_.emplace("Content-Length", std::to_string(json_data_.size()));
        std::string http_header_str = serialize_header();
        socket.write_some(boost::asio::buffer(http_header_str));
        return socket.write_some(boost::asio::buffer(json_data_));
    }
} // obelisk