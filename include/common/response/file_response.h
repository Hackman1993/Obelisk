//
// Created by Hackman.Lo on 7/5/2023.
//

#ifndef LIBRARYORGANIZED_FILE_RESPONSE_H
#define LIBRARYORGANIZED_FILE_RESPONSE_H
#include "http_response.h"
namespace obelisk {

    class file_response: public http_response{
    public:
        file_response(const std::string &path);
    protected:
        size_t write_content_(boost::asio::ip::tcp::socket& socket) override;
        std::string file_path_;
    };

} // obelisk

#endif //LIBRARYORGANIZED_FILE_RESPONSE_H
