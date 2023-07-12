//
// Created by Hackman.Lo on 6/25/2023.
//

#ifndef LIBRARYORGANIZED_JSON_RESPONSE_H
#define LIBRARYORGANIZED_JSON_RESPONSE_H

#include "http_response.h"
#include "boost/json.hpp"

namespace obelisk {


    class json_response : public http_response {
    public:
        json_response(boost::json::object object, std::size_t code);

        json_response(boost::json::array object, std::size_t  code);

    protected:
        size_t write_content_(boost::asio::ip::tcp::socket &socket) override;

        std::string json_data_;
    };

} // obelisk

#endif //LIBRARYORGANIZED_JSON_RESPONSE_H
