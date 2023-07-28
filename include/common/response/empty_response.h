//
// Created by Hackman.Lo on 7/26/2023.
//

#ifndef OBELISK_EMPTY_RESPONSE_H
#define OBELISK_EMPTY_RESPONSE_H
#include "http_response.h"
namespace obelisk {

    class empty_response : public http_response{
    public:
        empty_response(std::uint32_t code) : http_response(code) {}
        size_t write_content_(boost::asio::ip::tcp::socket& socket) override;
    };

} // obelisk

#endif //OBELISK_EMPTY_RESPONSE_H
