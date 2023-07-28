//
// Created by Hackman.Lo on 7/5/2023.
//

#include <filesystem>
#include <fstream>
#include "common/details/mime_types.h"
#include "exception/http_exception.h"
#include "common/response/file_response.h"

namespace obelisk {
    file_response::file_response(const std::string &path) : http_response(200), file_path_(path) {}

    size_t file_response::write_content_(boost::asio::ip::tcp::socket &socket) {
        std::size_t ret = 0;
        // Prepare Content Length
        auto file_size = std::filesystem::file_size(file_path_);
        headers_.emplace("Content-Length", std::to_string(file_size));

        // Prepare Content type
        std::string content_type = mime_types::lookup(file_path_);
        headers_.emplace("Content-Type", content_type);

        std::ifstream fstream(file_path_, std::ios::binary|std::ios::in);
        fstream.seekg(std::ios::beg);
        if(!fstream.is_open())
            throw exception::http_exception("Forbidden", 403);

        std::string http_header_str = serialize_header();
        ret += socket.write_some(boost::asio::buffer(http_header_str));
        char buffer[102400] = {0};
        while(!fstream.eof()){
            fstream.read(buffer, 102400);
            auto bytes_read = fstream.gcount();
            ret += socket.write_some(boost::asio::buffer(buffer, bytes_read));
        }
        return ret;
    }
} // obelisk