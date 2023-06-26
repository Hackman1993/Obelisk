#include <sstream>
#include "common/response/http_response.h"
#include "common/session/http_session.h"
namespace obelisk{
    std::string http_response::serialize_header(){
        std::ostringstream result;
        result << "HTTP/1.1 " << std::to_string(resp_code_) << " OK\r\n";
        for(auto& header: headers_){
            result << header.first.to_std() << ": " << header.second.to_std() << "\r\n";
        }
        result << "\r\n";
        return result.str();
    }

    std::size_t http_response::write_content_(boost::asio::ip::tcp::socket &socket) {
        std::string http_header_str = serialize_header();
        boost::asio::write(socket, boost::asio::buffer(http_header_str));
        return 0;
    }

    std::size_t http_response::write_content(std::shared_ptr<http_session> session) {
        return write_content_(session->socket_);
    }

}