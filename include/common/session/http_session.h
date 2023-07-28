//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_PLAIN_HTTP_SESSION_H
#define OBELISK_PLAIN_HTTP_SESSION_H

#include <iostream>
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include "../../exception/http_exception.h"
#include "../request/http_request.h"

using namespace std::chrono_literals;
namespace obelisk {
    class http_server;

    class http_response;

    class http_session : public std::enable_shared_from_this<http_session> {

    private:
        std::string uuid_;
        http_server &server_;
        boost::asio::ip::tcp::socket socket_;
        boost::asio::streambuf streambuf_;
        boost::asio::deadline_timer deadline_timer_;
    public:
        /**
         * @brief Construct a new http session object
         * @param uuid: Set uuid for this session, Used to keep the session in the memory
         * @param socket: The socket of this session
         * @param server: The server of this session
         * @description: When this object is created, it will start the timer, if expired, will force close the socket
         */
        http_session(std::string uuid, boost::asio::ip::tcp::socket &socket, http_server &server);
        /**
         * @brief Destroy the http session object
         * @description: Shutdown the socket, and cancel all the IO operation
         */
        ~http_session();
    protected:
        /*--------------------------------IO Functions----------------------------------------*/
        /**
         * @name: new_request_
         * @description: Start to handling new request
         */
        void new_request_();
        /**
         * @name: receive_header_
         * @description: Http receive logic, when we received \r\n\r\n, header_received_ will be called
         * @param request: The request pointer
         */
        void receive_header_(const std::shared_ptr<http_request> &request);
        /**
         * @name: header_received_
         * @description: Try to parse the http header, if failed, session will be closed, otherwise will start to receive http body if needed
         * @param ec: Passed by the async read lambda
         * @param transferred: Passed by the async read lambda
         * @param request: The request pointer
         */
        void header_received_(const boost::system::error_code &ec, std::size_t transferred, const std::shared_ptr<http_request>& request);
        /**
         * @name: receive_http_body_
         * @description: Try to receive the http body, if the content length is 0, will call http_body_received_ directly
         * @param request : The request pointer
         * @param iostream : The iostream pointer, Passed by the header_received_ function, the object might be http_temp_filestream or std::stringstream depends on the content length
         */
        void receive_http_body_(const std::shared_ptr<http_request>& request, const std::shared_ptr<std::iostream>& iostream);
        /**
         * @name: http_body_received_
         * @description: Write transferred bytes to the iostream, will call handle_request_ if data is fully received
         * @param ec: Passed by the async read lambda
         * @param transferred: Passed by the async read lambda
         * @param request: The request pointer
         * @param iostream: The iostream pointer, Passed by the receive_http_body_ function, the object might be http_temp_filestream or std::stringstream depends on the content length
         */
        void http_body_received_(const boost::system::error_code &ec, std::size_t transferred, const std::shared_ptr<http_request>& request, const std::shared_ptr<std::iostream>& iostream);
        /**
         * @name: request_timeout_
         * @description: When the timer expired, will be called, the session will be release forcely
         * @param e: The error code, passed by the deadline timer
         */
        void request_timeout_(const boost::system::error_code &e);
        /*--------------------------------Parse Functions----------------------------------------*/
        /**
         * @name: parse_http_package_header_
         * @description: Parse the http header
         * @param data: The data to be parsed
         * @param request: The request pointer
         * @return bool: true if success, otherwise false
         */
        static bool parse_http_package_header_(std::string_view data, const std::shared_ptr<http_request>& request);
        /**
         * @name: parse_multi_part_form_body_
         * @description: called from http_body_received_ if the content type is multipart/form-data
         * @param stream : The stream to be parsed
         * @param request : The request pointer
         */
        static void parse_multipart_form_body_(const std::shared_ptr<std::istream>& stream, const std::shared_ptr<http_request>& request);
        /**
         * @name: parse_urlencoded_form_body_
         * @description: called from http_body_received_ if the content type is application/x-www-form-urlencoded
         * @param stream : The stream to be parsed
         * @param request : The request pointer
         */
        static void parse_urlencoded_form_body_(const std::shared_ptr<std::istream>& stream, const std::shared_ptr<http_request>& request);
        /*--------------------------------Handle Functions----------------------------------------*/
        /**
         * @name: handle_request_
         * @description: Called from http_body_received_ if the data is fully received, then will match the trigger by following order:\n
         *               1. file\n
         *               2.middleware\n
         *               3. route\n
         *               if any handler returns a not null response pointer, the response will imeediately send back to the client
         * @param request: The request pointer
         */
        void handle_request_(const std::shared_ptr<http_request>& request);
        /**
         * @name: file_handle_
         * @description: Generate a file response, if the file is not found, will return null to perform the next handler
         * @param request: The request pointer
         * @return std::shared_ptr<http_response>: The response pointer, null if not found
         */
        static std::shared_ptr<http_response> file_handle_(const std::shared_ptr<http_request>& request);

        friend class http_response;
    };
}


#endif //OBELISK_PLAIN_HTTP_SESSION_H
