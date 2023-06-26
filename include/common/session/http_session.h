//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_PLAIN_HTTP_SESSION_H
#define OBELISK_PLAIN_HTTP_SESSION_H

#include <boost/asio.hpp>
#include "common/request/http_request.h"
#include "exception/http_exception.h"
#include "common/request/request_header_parser.h"
#include <iostream>

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
        // Create the session
        http_session(std::string uuid, boost::asio::ip::tcp::socket &socket, http_server &server);

        ~http_session();

        // Start the session
        void run();

    protected:
        /*--------------------------------IO Functions----------------------------------------*/
        void receive_header_();
        void header_received_(const boost::system::error_code &ec, std::size_t transferred, std::shared_ptr<http_request_raw> request);
        void receive_http_body_(std::shared_ptr<http_request_raw> request, std::shared_ptr<std::iostream> iostream);
        void http_body_received_(const boost::system::error_code &ec, std::size_t transferred, std::shared_ptr<http_request_raw> request, std::shared_ptr<std::iostream> iostream);
        /*--------------------------------Parse Functions----------------------------------------*/
        void parse_multipart_form_body_(std::shared_ptr<std::iostream> stream, std::shared_ptr<http_request_raw> request);


        void handle_request_(std::shared_ptr<http_request_raw> request);

        void request_timeout_(const boost::system::error_code &e);

        friend class http_response;


//      try {
//        while (true) {
//
//
//


//
//
//
//
//          {
//            std::uint64_t bytes_remain = request.content_length_;
//            auto buffer_length_ = std::min<std::uint32_t>(bytes_remain, server.body_block_length());
//            auto buffer_length = std::clamp(buffer_length_, 0u, server.body_block_length());
//            sbuf.prepare(buffer_length - sbuf.in_avail());
//            std::variant< std::nullptr_t, std::string, std::size_t> receiving_mode;
//            std::size_t bytes_consumed = 0;
//            std::function<std::size_t (http_server&, http_request&, std::string_view, std::variant< std::nullptr_t, std::string, std::size_t>&)> handler;
//
//            // Content Type Parse
//            if(request.headers_["Content-Type"].icontains("multipart/form-data")){
//              receiving_mode = "\r\n";
//              std::string boundary_ = "--" + request.boundary_;
//              for (bool package_ended = false; !package_ended; ) {
//                // Notice: these string will be used only if this is a file except the param_name
//                std::string content_type, origin_name, temp_file_name, param_name;
//                bool is_file = false;
//
//                // Receiving Meta Data Of Multipart Request Body
//                for(;;){
//                  transferred = co_await boost::asio::async_read_until(socket, sbuf, "\r\n", boost::asio::use_awaitable);
//                  std::string_view header_data_v(boost::asio::buffer_cast<const char *>(sbuf.data()), transferred);
//                  if (header_data_v == boundary_ +"\r\n") {
//                    bytes_remain -= header_data_v.size();
//                    sbuf.consume(transferred);
//                    continue;
//                  }
//                  if(header_data_v == boundary_ + "--\r\n") {
//                    bytes_remain -= header_data_v.size();
//                    sbuf.consume(header_data_v.size());
//                    package_ended = true;
//                  }
//
//                  if (!header_data_v.ends_with("\r\n")) throw std::runtime_error("Invalid Request Body");
//                  if(header_data_v == "\r\n") {
//                    bytes_remain -= header_data_v.size();
//                    sbuf.consume(transferred);
//                    break;
//                  }
//
//                  std::pair<std::string, std::string> first_round_result;
//                  bool parse_result = phrase_parse(header_data_v.begin(), header_data_v.end(), meta_item_parser, space, first_round_result);
//                  if (!parse_result) throw std::runtime_error("Parse Body Metadata Failed;");
//
//                  // Extract Data In Content-Disposition
//                  if(boost::iequals(first_round_result.first, "Content-Disposition"))
//                  {
//                    std::unordered_map<std::string, std::string, ihash, iequal> metas;
//                    parse_result = phrase_parse(first_round_result.second.begin(), first_round_result.second.end(), no_case["form-data"] >> ";" >> disposition_parser % ';', space, metas);
//                    if(!metas.contains("name")) throw std::runtime_error("Form data doesn't have a name");
//                    else param_name = metas["name"];
//                    if(metas.contains("filename")){
//                      using namespace boost::uuids;
//                      origin_name = metas["filename"], is_file = true;
//                      std::string filename = to_string(random_generator()()) + std::filesystem::path(metas["filename"]).extension().string();
//                      temp_file_name = "./temp/" + filename;
//                    }
//                  }else if(boost::iequals(first_round_result.first, "Content-Type")) content_type = first_round_result.second;
//                  bytes_remain -= transferred;
//                  sbuf.consume(transferred);
//                }
//
//                // Preparing Output Stream
//                std::shared_ptr<std::ostream> output;
//                if(is_file) output = std::make_shared<std::ofstream>(temp_file_name, std::ios::binary);
//                else output = std::make_shared<std::ostringstream>();
//                if(output->bad()) throw std::runtime_error("Failed to open output stream");
//
//                // Receiving Body Data
//                for(bool should_end = false; !should_end;){
//                  // Calculate Buffer Size
//                  buffer_length_ = std::min<std::uint32_t>(bytes_remain, server.body_block_length());
//                  buffer_length = std::clamp(buffer_length_, 0u, server.body_block_length());
//                  sbuf.prepare(buffer_length - sbuf.in_avail());
//
//                  transferred = co_await boost::asio::async_read_until(socket, sbuf, "\r\n" + boundary_, boost::asio::use_awaitable);
//                  std::string_view body_data(boost::asio::buffer_cast<const char *>(sbuf.data()), transferred);
//
//                  should_end = body_data.ends_with("\r\n" + boundary_);
//
//                  // We Remain Some Bytes In Buffer To Prevent Boundary Being Cut
//                  std::streamsize length = should_end? body_data.size() - boundary_.size() - 2: body_data.size() - boundary_.size();
//                  output->write(body_data.data(), length);
//                  sbuf.consume(should_end? length+2: length);
//                  bytes_remain -= should_end? length+2: length;
//                }
//
//                if(is_file){
//                  dynamic_cast<std::ofstream*>(output.get())->close();
//                  request.file_bag_.emplace(param_name, request_file{origin_name, temp_file_name, content_type});
//                }
//                else
//                  request.request_params_.set(param_name, dynamic_cast<std::ostringstream*>(output.get())->str());
//
//                if(bytes_remain < boundary_.size()+2) throw std::runtime_error("Request Corrupted");
//              }
//            }
//          }
//        // TODO: Handle Request
//
//        http_request req(request);
//        std::shared_ptr<http_response> response;
//        try{
//          for(auto & item : server.middlewares()){
//            response = item.handle(req);
//          }
//          auto result = server_.router().handle(req);
//          if(result) return *result;
//        }catch (exception::http_exception& e){
//          sahara::string error = fmt::format("Server Error: {}", e.message());
//          sahara::string data = fmt::format("{{ \"code\": \"{}\", \"message\": \"{}\", \"data\": null}}", e.code(), e.message());
//          return string_response(e.code(), data.to_std(), req.version());
//        } catch (std::exception& e){
//          return string_response(500, e.what(), req.version());
//        }
//        return string_response(404, "Not Found!", req.version());
//        }
//      } catch (std::exception &e) {
//        std::cout << e.what() << std::endl;
//      }
//      socket.close();
//    }

    };
}


#endif //OBELISK_PLAIN_HTTP_SESSION_H
