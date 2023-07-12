#include <fstream>
#include <utility>
#include <iostream>
#include <sahara/utils/uuid.h>
#include "server/http_server.h"
#include "common/session/http_session.h"
#include "common/response/file_response.h"
#include "common/response/json_response.h"
#include <boost/fusion/adapted/std_pair.hpp>
#include "common/details/http_temp_filestream.h"
using namespace std::chrono_literals;

BOOST_FUSION_ADAPT_STRUCT(obelisk::http_request_header_begin_io, method_, target_, protocol_);
BOOST_FUSION_ADAPT_STRUCT(obelisk::http_pair_data_io, key_, value_);
BOOST_FUSION_ADAPT_STRUCT(obelisk::http_package_header_io, mat_, headers_);
namespace obelisk {
    http_session::http_session(std::string uuid, boost::asio::ip::tcp::socket &socket, http_server &server) : socket_(std::move(socket)), server_(server), uuid_(std::move(uuid)), deadline_timer_(socket_.get_executor()), streambuf_(1048576) {
        deadline_timer_.expires_from_now(boost::posix_time::seconds(server_.recv_timeout().to_ms() / 1000));
        deadline_timer_.async_wait([this](const boost::system::error_code &e) { request_timeout_(e); });
        new_request_();
    }

    http_session::~http_session() {
        if (socket_.is_open()) {
            try {
                socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            } catch (std::exception &) {}
        }
    }

    void http_session::new_request_() {
        std::shared_ptr<http_request> request_raw = std::make_shared<http_request>();
        streambuf_.consume(streambuf_.size());
        receive_header_(request_raw);
    }

    void http_session::receive_header_(const std::shared_ptr<http_request> &request) {
        try {
            socket_.async_read_some(streambuf_.prepare(10240), [this, request](const boost::system::error_code &ec, std::size_t transferred) { header_received_(ec, transferred, request); });
        } catch (std::exception &e) {
            server_.release_connection(uuid_);
        }
    }

    void http_session::header_received_(const boost::system::error_code &ec, std::size_t transferred, const std::shared_ptr<http_request> &request) {
        if (ec) {
            if (ec.value() == 995) return;
            server_.release_connection(uuid_);
            return;
        }
        try {
            streambuf_.commit(transferred);
            std::string_view received_data(boost::asio::buffer_cast<const char *>(streambuf_.data()), transferred);
            if (server_.header_max_length() <= received_data.length() && !received_data.contains("\r\n\r\n")){
                server_.release_connection(uuid_);
                return;
            }
            else if (!received_data.contains("\r\n\r\n")) {
                receive_header_(request);
                return;
            } else {
                auto separator = boost::algorithm::find_first(received_data, "\r\n\r\n");
                received_data = std::string_view(received_data.data(), separator.end() - received_data.begin());
                if (!parse_http_package_header_(received_data, request)) throw std::runtime_error("Invalid HTTP Header");
                streambuf_.consume(received_data.length());
                // TODO: Add Content-Length Check
//                if (request->content_length() > request->maximum_body_size_)
//                    throw std::runtime_error("Request Body Too Big");
                if (request->content_length() <= 0) {
                    handle_request_(request);
                    return;
                }
                std::shared_ptr<std::iostream> iostream;
                // If content length less than 10 MB, we don't need a file buffer
                if(request->content_length_ < 1024*1024*10)
                    iostream = std::make_shared<std::stringstream>();
                // If content is long , we need a file to store http content
                else{
                    auto path = "./" + sahara::utils::uuid::generate();
                    iostream = std::make_shared<http_temp_fstream>(path);
                }
                // Content may stick with http header, so write the remain data into buffer
                auto body_data_in_buffer = std::min(request->content_length(), streambuf_.size());
                iostream->write(boost::asio::buffer_cast<const char *>(streambuf_.data()), body_data_in_buffer);
                iostream->flush();
                // Calculate data haven't received
                request->bytes_remain_ = static_cast<std::streamsize>(request->content_length() - body_data_in_buffer);
                // Consumed the data has been written
                streambuf_.consume(body_data_in_buffer);
                receive_http_body_(request, iostream);
            }
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }

    void http_session::receive_http_body_(const std::shared_ptr<http_request> &request, const std::shared_ptr<std::iostream> &iostream) {
        try {
            // Calculate the bytes should receive, if bytes_remains is 0, will call http_body_received_ immediately
            std::size_t bytes_remains = std::min<std::size_t>(request->bytes_remain_, 10240);
            socket_.async_read_some(streambuf_.prepare(bytes_remains), [this, request, iostream](const boost::system::error_code &ec, std::size_t transferred) {
                http_body_received_(ec, transferred, request, iostream);
            });
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }

    }

    void http_session::http_body_received_(const boost::system::error_code &ec, std::size_t transferred, const std::shared_ptr<http_request> &request, const std::shared_ptr<std::iostream> &iostream) {
        if (ec) {
            if (ec.value() == 995) return;
            std::cout << ec.what() << std::endl;
            server_.release_connection(uuid_);
            return;
        }
        try {
            if (transferred > 0) {
                streambuf_.commit(transferred);
                // Calculate Actual Package Data , Prevent Sticky packets
                auto actual_package_size = std::min<std::size_t>(request->bytes_remain_, transferred);
                request->bytes_remain_ -= static_cast<std::size_t>(actual_package_size);
                std::string_view received_data(boost::asio::buffer_cast<const char *>(streambuf_.data()), actual_package_size);
                iostream->write(received_data.data(), static_cast<std::streamsize>(received_data.size()));
                iostream->flush();
                streambuf_.consume(received_data.size());
            }
            if (request->bytes_remain_ > 0) receive_http_body_(request, iostream);
            else {
                // TODO: Handle Http Body By Content Type
                if (request->headers_.contains("Content-Type") && request->headers_["Content-Type"] == "application/json") {
                } else if (request->headers_["Content-Type"].icontains("multipart/form-data")) {
                    parse_multipart_form_body_(iostream, request);
                } else if (request->headers_["Content-Type"].icontains("application/x-www-form-urlencoded")) {
                    parse_urlencoded_form_body_(iostream, request);
                } else {

                }
                handle_request_(request);
            }
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }

    }

    void http_session::handle_request_(const std::shared_ptr<http_request>& request) {
        deadline_timer_.expires_from_now(boost::posix_time::seconds(server_.recv_timeout().to_ms() / 1000));
        deadline_timer_.async_wait([this](const boost::system::error_code &e) { request_timeout_(e); });
        http_request req(*request);
        std::shared_ptr<http_response> response;
        try {
            // Try Match File Goes First
            if (!response)
                response = file_handle_(request);
            // Middlewares
            for (auto &item: server_.middlewares()) {
                response = item.handle(req);
                if (response) {
                    break;
                }
            }
            // Router Match
            if (!response)
                response = server_.router().handle(req);
            // Nothing Matched , return a 404 Error
            if (!response) {
                response = std::make_shared<json_response>(boost::json::object({{"code",    404},
                                                                                {"message", "Not Found"},
                                                                                {"data",    boost::json::value()}}), 404);
            }
        } catch (exception::http_exception &e) {
            response = std::make_shared<json_response>(boost::json::object({{"code",    e.code()},
                                                                            {"message", e.what()},
                                                                            {"data",    boost::json::value()}}), e.code());
        } catch (std::exception &e) {
            response = std::make_shared<json_response>(boost::json::object({{"code",    500},
                                                                            {"message", e.what()},
                                                                            {"data",    boost::json::value()}}), 500);
        }
        try {
            // Write Content
            if (response)
                response->write_content(shared_from_this());
            else
                std::cout << "Null Response After All Possible Handle" << std::endl;
            if(request->headers_.contains("Connection") && request->headers_["Connection"].icontains("keep-alive"))
                new_request_();
            else
                server_.release_connection(uuid_);
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            server_.release_connection(uuid_);
        }
    }

    void http_session::request_timeout_(const boost::system::error_code &e) {
        if (e && e.value() == 995) return;
        if (e)
            std::cout << e.what() << std::endl;
        server_.release_connection(uuid_);
    }

    auto boundary_parser_ = no_case["multipart/form-data; boundary="] >> (+~char_("\r\n"));
    void http_session::parse_multipart_form_body_(const std::shared_ptr<std::istream> &stream, const std::shared_ptr<http_request>& request) {
        if(!request->headers_.contains("Content-Type")) return;
        std::string boundary;
        std::string content_type_data = request->headers_["Content-Type"].to_std();
        bool boundary_parse_result = parse(content_type_data.begin(), content_type_data.end(), boundary_parser_, boundary);
        if(!boundary_parse_result){
            std::cout << "Boundary data parse failed" << std::endl;
            return;
        }
        std::string boundary_end_data = "--" + boundary + "--\r";
        std::string boundary_split_data = "--" + boundary;
        stream->seekg(std::ios::beg);
        try {
            while (!stream->eof()) {
                // Read Boundary
                {
                    std::string boundary_line;
                    std::getline(*stream, boundary_line);
                    if (boundary_line == boundary_end_data) return;
                    if (boundary_line.empty()) {
                        std::cout << "Invalid multipart from data:" << boundary_line << std::endl;
                        return;
                    }
                }
                // Read Meta Data
                bool is_file = false;
                std::string meta_name, filename, content_type, temp_file_path;
                std::unique_ptr<std::ofstream> fstream = nullptr;
                {
                    std::string meta_line;
                    std::getline(*stream, meta_line);
                    meta_line.resize(meta_line.size() - 1);
                    if (meta_line.empty()) {
                        std::cout << "Invalid Form Meta Data" << std::endl;
                        return;
                    }
                    auto name_parser = (no_case["name"] > "=" > '"' > lexeme[*(char_ - '"')] > '"')[([&meta_name](auto &ctx) {
                        meta_name = _attr(ctx);
                        _pass(ctx) = true;
                    })];
                    auto filename_parser = (no_case["filename"] > "=" > '"' > lexeme[*(char_ - '"')] > '"')[([&filename, &is_file, &fstream, &temp_file_path](auto &ctx) {
                        is_file = true;
                        filename = _attr(ctx);
                        temp_file_path = "./" + sahara::utils::uuid::generate();
                        fstream = std::make_unique<std::ofstream>(temp_file_path, std::ios_base::binary);
                        _pass(ctx) = true;
                    })];
                    auto disposition_data_parser = lit("form-data") | name_parser | filename_parser;
                    bool parse_result = phrase_parse(meta_line.begin(), meta_line.end(), lit("Content-Disposition") >> ":" >> (disposition_data_parser % ';'), space);
                    if (!parse_result) {
                        std::cout << "Body Data Parse Failed!" << std::endl;
                        return;
                    }
                }
                // If content is a file, should read content type
                if (is_file) {
                    std::string content_type_line;
                    std::getline(*stream, content_type_line);
                    if (content_type_line.empty() || content_type_line == "\r") {
                        std::cout << "Form data was a file but no content type was specified" << std::endl;
                        return;
                    }
                    auto content_type_parser = no_case["Content-Type"] > ":" > lexeme[*~char_("\r\n")][([&content_type](auto &ctx) {
                        content_type = _attr(ctx);
                        _pass(ctx) = true;
                    })] >> "\r";
                    bool parse_result = parse(content_type_line.begin(), content_type_line.end(), content_type_parser);
                    if (!parse_result) {
                        std::cout << "Body Content Type Parse Failed!" << std::endl;
                        return;
                    }
                }
                // The Empty Line Before Data
                {
                    std::string empty_line;
                    std::getline(*stream, empty_line);
                    if (empty_line != "\r") {
                        std::cout << "Unexpected Line :" << empty_line << std::endl;
                        return;
                    }
                }

                // If not a file, read data
                if (!is_file) {
                    std::string data_line;
                    std::getline(*stream, data_line);
                    if (data_line.empty()) {
                        std::cout << "Invalid Multipart Form Data" << std::endl;
                        return;
                    }
                    data_line.resize(data_line.length() - 1);
                    request->request_params_.set(meta_name, data_line);
                    continue;
                }

                boost::asio::streambuf sbuf_;
                sbuf_.prepare(10240);
                char data[10240] = {0};
                std::string_view file_buffer;
                do {
                    auto bytes_read = stream->readsome(data, 10240);
                    sbuf_.sputn(data, bytes_read);
                    file_buffer = std::string_view(boost::asio::buffer_cast<const char *>(sbuf_.data()), sbuf_.size());
                    if (!file_buffer.contains(boundary_split_data)) {
                        fstream->write(file_buffer.data(), static_cast<std::streamsize>(file_buffer.size() - boundary.size() - 5));
                        sbuf_.consume(sbuf_.size() - boundary.size() - 5);
                    } else {
                        auto range = boost::algorithm::find_first(file_buffer, std::string("\r\n--" + boundary));
                        std::string_view file_content_part = std::string_view(file_buffer.data(), range.begin() - file_buffer.begin());
                        fstream->write(file_content_part.data(), static_cast<std::streamsize>(file_content_part.size()));
                        sbuf_.consume(file_content_part.size() + 2);
                    }
                } while (!file_buffer.contains(boundary_split_data));
                stream->seekg(-sbuf_.in_avail(), std::ios::cur);
                fstream->flush();
                fstream->close();
                request->file_bag_.emplace(meta_name, request_file{filename, temp_file_path, content_type});
            }
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }

    std::shared_ptr<http_response> http_session::file_handle_(const std::shared_ptr<http_request> &request) {
        std::string base_dir = "./webroot";
        std::filesystem::path target_path = absolute(std::filesystem::path(base_dir).concat(request->target_.to_std()));
        std::filesystem::path relative_path = relative(target_path, base_dir);

        if (!std::filesystem::exists(target_path) || std::filesystem::is_directory(target_path))
            return nullptr;
        if (relative_path.string().starts_with(".."))
            throw exception::http_exception(403, "Forbidden");
        return std::make_shared<file_response>(relative_path.string());
    }

    auto http_params_parser_ = rule<class mat_parser, http_pair_data_io>{"HttpPairParser"} = *~char_("=&") >> -lit("=") >> *~char_("&");
    void http_session::parse_urlencoded_form_body_(const std::shared_ptr<std::istream> &stream, const std::shared_ptr<http_request>& request) {
        boost::asio::streambuf sbuf;
        stream->seekg(std::ios::beg);
        char data[102400] = {0};
        while (!stream->eof()) {
            stream->read(data, 102400);
            auto bytes_read = stream->gcount();
            sbuf.sputn(data, bytes_read);
        }
        std::string_view body = std::string_view(boost::asio::buffer_cast<const char *>(sbuf.data()), sbuf.size());
        std::vector<http_pair_data_io> params;
        bool result = parse(body.begin(), body.end(),http_params_parser_%'&', params);
        if(!result ) {
            std::cout << "Url Encoded Body Parse Failed!" << std::endl;
            return;
        }
        for(auto & param : params){
            if(param.key_.empty() && param.value_.empty()) continue;
            request->request_params_.set(sahara::string(param.key_).url_decode_copy(), sahara::string(param.value_).url_decode_copy());
        }

    }
    auto mat_parser_ = rule<class mat_parser, http_request_header_begin_io>{"MATParser"} = +~char_(" \r\n") >> " " >> +~char_(" \r\n") >> " " >> no_case["HTTP/"] >> +~char_(" \r\n") >> "\r\n";
    auto http_headers_parser_ = rule<class mat_parser, http_pair_data_io>{"HttpHeadersParser"} = +~char_(":\r\n") >> ": " >> (+~char_("\r\n")) >> "\r\n";
    auto http_package_header_parser_ = rule<class mat_parser, http_package_header_io>{"HttpPackageHeaderParser"} = mat_parser_ >> +(http_headers_parser_) >> "\r\n";

    bool http_session::parse_http_package_header_(std::string_view data, const std::shared_ptr<http_request> &request) {
        http_package_header_io header_io;
        auto result = parse(data.begin(), data.end(), http_package_header_parser_, header_io);
        request->method_ = header_io.mat_.method_;
        request->protocol_ = header_io.mat_.protocol_;
        request->target_ = header_io.mat_.target_;
        for(auto & header : header_io.headers_){
            request->headers_.emplace(header.key_, header.value_);
        }
        if(request->headers_.contains("Content-Length"))
            request->content_length_ = std::stoull(request->headers_["Content-Length"]);
        if(request->target_.contains("?")){
            request->path_ = request->target_.substr(0, request->target_.find_first("?"));
            // Parse Url Params
            std::string url_params = request->target_.substr(request->target_.find_first("?") + 1);
            std::vector<http_pair_data_io> params;
            bool params_parse_result = parse(url_params.begin(), url_params.end(),http_params_parser_%'&', params);
            if(!params_parse_result ) return false;
            for(auto & param : params){
                if(param.key_.empty() && param.value_.empty()) continue;
                request->request_params_.set(sahara::string(param.key_).url_decode_copy(), sahara::string(param.value_).url_decode_copy());
            }
        }
        return result;
    }


}