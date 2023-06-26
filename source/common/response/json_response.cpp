//
// Created by Hackman.Lo on 6/25/2023.
//

#include "common/response/json_response.h"
#include <boost/asio.hpp>
namespace obelisk {
    json_response::json_response(unsigned int code, const boost::json::object& object): http_response(code) {
        headers_["Content-Type"] = sahara::string("application/json");
        json_data_ = boost::json::serialize(object);
        headers_["Content-Length"] = std::to_string(json_data_.size());
    }

    json_response::json_response(unsigned int code, const boost::json::array& object): http_response(code) {
        headers_["Content-Type"] = sahara::string("application/json");
        json_data_ = boost::json::serialize(object);
        headers_["Content-Length"] = std::to_string(json_data_.size());
    }

    size_t json_response::write_content_(boost::asio::ip::tcp::socket &socket) {
        return socket.write_some(boost::asio::buffer(json_data_));
    }
} // obelisk