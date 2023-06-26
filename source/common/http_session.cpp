#include "server/http_server.h"
#include "common/session/http_session.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <utility>
#include "utils/uuid.h"
#include <boost/asio/posix/stream_descriptor.hpp>
#include "common/details/http_temp_stream.h"

using namespace std::chrono_literals;
namespace obelisk {
  http_session::http_session(std::string uuid, boost::asio::ip::tcp::socket &socket, http_server &server) : socket_(std::move(socket)), server_(server),
                                                                                                            uuid_(std::move(uuid)),
                                                                                                            deadline_timer_(socket_.get_executor()),
                                                                                                            streambuf_(1048576) {
    deadline_timer_.expires_from_now(boost::posix_time::seconds(server_.recv_timeout().to_ms() / 1000));
    deadline_timer_.async_wait([this](const boost::system::error_code &e) { request_timeout_(e); });

  }

  http_session::~http_session() {
    socket_.close();
    std::cout << "Socket Shutting Down" << std::endl;
  }

  void http_session::receive_header_() {
    std::shared_ptr<http_request_raw> request_raw = std::make_shared<http_request_raw>();
    socket_.async_read_some(streambuf_.prepare(10240), [this, request_raw](const boost::system::error_code &ec, std::size_t transferred) {
      header_received_(ec, transferred, request_raw);
    });
  }

  void http_session::header_received_(const boost::system::error_code &ec, std::size_t transferred, std::shared_ptr<http_request_raw> request) {
    if (ec) {
      server_.release_connection(uuid_);
      return;
    }
    streambuf_.commit(transferred);
    std::string_view received_data(boost::asio::buffer_cast<const char *>(streambuf_.data()), transferred);
    if (server_.header_max_length() == received_data.length() && !received_data.contains("\r\n\r\n"))
      socket_.close();
    else if (!received_data.contains("\r\n\r\n")) {
      boost::asio::async_read(socket_, streambuf_, [this, request](const boost::system::error_code &ec, std::size_t transferred) {
        header_received_(ec, transferred, request);
      });
    } else {
      auto separator = boost::algorithm::find_first(received_data, "\r\n\r\n");
      received_data = std::string_view(received_data.data(), separator.end() - received_data.begin());

      if (!parse_request_header(received_data, *request)) throw std::runtime_error("Invalid HTTP Header");
      streambuf_.consume(received_data.length());

      // TODO: maximum_body_size_
      if (request->content_length_ > request->maximum_body_size_) throw std::runtime_error("Request Body Too Big");
      if (request->content_length_ <= 0) {
        handle_request_(request);
      }
        // Receiving Http Body
      else {
        std::shared_ptr<std::iostream> iostream = nullptr;
        if (request->content_length_ > request->maximum_body_size_) {
          iostream = std::make_shared<std::fstream>("./" + sahara::utils::uuid::generate());
        } else {
          iostream = std::make_shared<std::stringstream>();
        }
        iostream->write(boost::asio::buffer_cast<const char *>(streambuf_.data()), streambuf_.size());
        iostream->flush();
        request->bytes_remains_ = request->content_length_ - streambuf_.size();
        receive_http_body_(request, iostream);
      }
    }
  }

  void http_session::receive_http_body_(std::shared_ptr<http_request_raw> request, std::shared_ptr<std::iostream> iostream) {
    std::size_t bytes_remains = std::min<std::size_t>(request->bytes_remains_, 10240);
    socket_.async_read_some(streambuf_.prepare(bytes_remains), [this, request, iostream](const boost::system::error_code &ec, std::size_t transferred) {
      http_body_received_(ec, transferred, request, iostream);
    });
    //boost::asio::async_read(socket_, streambuf_, boost::asio::transfer_exactly(bytes_remains), );

  }

  void http_session::http_body_received_(const boost::system::error_code &ec, std::size_t transferred, std::shared_ptr<http_request_raw> request,
                                         std::shared_ptr<std::iostream> iostream) {
    if (ec) {
      std::cout << ec.what() << std::endl;
      server_.release_connection(uuid_);
      return;
    }
    if (transferred > 0) {
      streambuf_.commit(transferred);
      request->bytes_remains_ -= transferred;
      // Calculate Actual Package Data , Prevent Sticky packets
      auto actual_package_size = std::min<std::size_t>(request->bytes_remains_, transferred);
      std::string_view received_data(boost::asio::buffer_cast<const char *>(streambuf_.data()), actual_package_size);
      iostream->write(received_data.data(), received_data.size());
      iostream->flush();
    }
    if (request->bytes_remains_ > 0) receive_http_body_(request, iostream);
    else {
      // TODO: Handle Http Body By Content Type
      if (request->headers_.contains("Content-Type") && request->headers_["Content-Type"] == "application/json") {
      } else if (request->headers_["Content-Type"].icontains("multipart/form-data")) {
        parse_multipart_form_body_(iostream, request);
      } else if (request->headers_["Content-Type"].icontains("application/x-www-form-urlencoded")) {
      } else {

      }

    }

  }

  void http_session::handle_request_(std::shared_ptr<http_request_raw> request) {
    deadline_timer_.expires_from_now(boost::posix_time::seconds(server_.recv_timeout().to_ms() / 1000));
    deadline_timer_.async_wait([this](const boost::system::error_code &e) { request_timeout_(e); });
    http_request req(*request);
    std::shared_ptr<http_response> response;
    try {
      for (auto &item: server_.middlewares()) {
        response = item.handle(req);
      }
      auto result = server_.router().handle(req);
      //if (result) return *result;
    } catch (exception::http_exception &e) {
      sahara::string error = fmt::format("Server Error: {}", e.message());
      sahara::string data = fmt::format(R"({{ "code": "{}", "message": "{}", "data": null}})", e.code(), e.message());
    } catch (std::exception &e) {
    }

    receive_header_();
  }

  void http_session::run() {
    receive_header_();
  }

  void http_session::request_timeout_(const boost::system::error_code &e) {
    if (e) {
      if (e.value() != 995)
        std::cout << e.to_string() << std::endl;
      deadline_timer_.cancel();
      return;
    }
    socket_.cancel();
  }

  void http_session::parse_multipart_form_body_(std::shared_ptr<std::iostream> stream, std::shared_ptr<http_request_raw> request) {
    using namespace boost::spirit::x3;
    stream->seekg(std::ios::beg);
    while (!stream->eof()) {
      // Read Boundary
      {
        std::string boundary_line;
        std::getline(*stream, boundary_line);
        if(boundary_line == ("--" + request->boundary_ + "--\r")) return;
        if(boundary_line.empty()) {
          std::cout << "Invalid multipart from data" << std::endl;
          return;
        }
      }
      // Read Meta Data
      bool is_file = false;
      std::string meta_name, filename, content_type;
      std::unique_ptr<http_temp_stream> fstream = nullptr;
      {
        using namespace boost::spirit::x3;
        std::string meta_line;
        std::getline(*stream, meta_line);
        meta_line.resize(meta_line.size()-1);
        if(meta_line.empty()){
          std::cout << "Invalid Form Meta Data" << std::endl;
          return;
        }
        auto name_parser = (no_case["name"] > "=" > '"' > lexeme[*(char_ - '"')] > '"')[([&meta_name](auto &ctx) {
          meta_name = _attr(ctx);
          _pass(ctx) = true;
        })];
        auto filename_parser =(no_case["filename"] > "=" > '"' > lexeme[*(char_ - '"')] > '"')[([&filename, &is_file, &fstream](auto &ctx) {
          is_file = true;
          filename = _attr(ctx);
          fstream = std::make_unique<std::ofstream>("./" + sahara::utils::uuid::generate(), std::ios_base::binary);
          _pass(ctx) = true;
        })];
        auto disposition_data_parser = lit("form-data") | name_parser | filename_parser;
        bool parse_result = phrase_parse(meta_line.begin(), meta_line.end(),  lit("Content-Disposition") >> ":" >> (disposition_data_parser%';') , space);
        if(!parse_result)
        {
          std::cout << "Body Data Parse Failed!" << std::endl;
          return;
        }
      }
      // If content is a file, should read content type
      if(is_file){
        std::string content_type_line;
        std::getline(*stream, content_type_line);
        if(content_type_line.empty()) {
          std::cout << "Form data was a file but no content type was specified" << std::endl;
          return;
        }
        using namespace boost::spirit::x3;
        auto content_type_parser = no_case["Content-Type"] > ":" > lexeme[*~char_("\r\n")][([&content_type](auto &ctx) {
          content_type = _attr(ctx);
          _pass(ctx) = true;
        })] >> "\r";
        bool parse_result = parse(content_type_line.begin(), content_type_line.end(), content_type_parser);
        if(!parse_result)
        {
          std::cout << "Body Content Type Parse Failed!" << std::endl;
          return;
        }
      }
      // The Empty Line Before Data
      {
        std::string empty_line;
        std::getline(*stream, empty_line);
        if(empty_line !="\r"){
          std::cout << "Unexpected Line :" << empty_line << std::endl;
          return;
        }
      }

      // If not a file, read data
      if(!is_file){
        std::string data_line;
        std::getline(*stream, data_line);
        if(data_line.empty()){
          std::cout << "Invalid Multipart Form Data" << std::endl;
          return;
        }
        data_line.resize(data_line.length()-1);
        request->request_params_.set(meta_name, data_line);
        continue;
      }

      boost::asio::streambuf sbuf_;
      sbuf_.prepare(10240);
      std::string boundary_check = "--"+request->boundary_;
      char data[10240] = {0};
      std::string_view file_buffer;
      do{
        auto bytes_read = stream->readsome(data, 10240);
        sbuf_.sputn(data, bytes_read);
        file_buffer = std::string_view(boost::asio::buffer_cast<const char*>(sbuf_.data()), sbuf_.size());
        if(!file_buffer.contains(boundary_check))
        {
          fstream->write(file_buffer.data(), file_buffer.size() - request->boundary_.size() -5);
          sbuf_.consume(sbuf_.size() - request->boundary_.size() -5);
        }else{
          auto range = boost::algorithm::find_first(file_buffer, std::string("\r\n--" + request->boundary_));
          std::string_view file_content_part = std::string_view(file_buffer.data(), range.begin() - file_buffer.begin());
          fstream->write(file_content_part.data(), file_content_part.size());
          sbuf_.consume(file_content_part.size()+2);
        }
      }while(!file_buffer.contains(boundary_check));
      stream->seekg(-sbuf_.in_avail(), std::ios::cur);
      fstream->flush();
      fstream->close();
      request->file_bag_.emplace(meta_name, request_file{filename, fstream->path(), content_type});
    }
  }


}