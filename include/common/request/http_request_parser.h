//
// Created by Hackman.Lo on 3/20/2023.
//

#ifndef OBELISK_HTTP_REQUEST_PARSER_H
#define OBELISK_HTTP_REQUEST_PARSER_H

#include <iostream>
#include <functional>
#include "http_request.h"
#include "common/details/request_file.h"
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/coroutine2/coroutine.hpp>
#include <boost/url.hpp>
#include "coder/url.h"

namespace obelisk {
  using namespace boost::beast;
  using namespace boost::spirit::x3;

  class http_request_parser : public http::basic_parser<true> {
  public:
    void url_encoded_body_parser(boost::coroutines2::coroutine<void>::push_type& yield){
      while(remain_) yield();
      std::string_view buffer_scope(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size());
      auto result = boost::urls::parse_origin_form(buffer_scope);
      streambuf_.consume(buffer_scope.size());
    }
    void form_data_body_parser(boost::coroutines2::coroutine<void>::push_type& yield)
    {
      for(;;){
        while(!std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size()).contains("\r\n")) {
          yield();
        }

        // If it's end boundary, exit thread
        std::string_view parsed_data(boost::asio::buffer_cast<const char*>(streambuf_.data()), std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size()).find("\r\n"));
        if(parsed_data == "--" + boundary_ + "--") break;
        streambuf_.consume(parsed_data.size()+2);

        // Reading Form Meta Data
        std::unordered_map<std::string, std::string, no_case_hash, no_case_equal> meta_data;
        for(;;){
          while(!std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size()).contains("\r\n")) {
            yield();
          }

          // Reading Form Meta Data
          if(std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size()).starts_with("\r\n"))
          {
            streambuf_.consume(2);
            break;
          }
          parsed_data = std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()), std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size()).find("\r\n"));
          std::string_view header = parsed_data.substr(0, parsed_data.find(':'));
          std::string_view value = parsed_data.substr(parsed_data.find(':')+1, parsed_data.size()-1);
          meta_data.emplace(header, value);
          streambuf_.consume(parsed_data.length()+2);
        }

        // Read Body Data
        std::unordered_map<std::string, boost::optional<std::string>, no_case_hash, no_case_equal> block_meta_data;
        auto parser = (+~char_(";=") >> -(lit("=") >> '\"' >> +~char_("\"") >> "\""))%";";
        bool parse_result  = boost::spirit::x3::phrase_parse(meta_data["Content-Disposition"].begin(),meta_data["Content-Disposition"].end(), parser, ascii::space, block_meta_data);
        if(!parse_result)
          std::cout << "Parse_failed:" << meta_data["Content-Disposition"] << std::endl;

        if(block_meta_data.contains("filename")){
            sahara::string file_param_name = block_meta_data["name"].get_value_or("");
          std::filesystem::path tmp_dir= "./";
          std::filesystem::path original_name(block_meta_data["filename"].get_value_or(""));
          std::filesystem::path temp_filepath = tmp_dir.append(boost::uuids::to_string(boost::uuids::random_generator()()) + original_name.extension().string());
          std::fstream stream(temp_filepath, std::ios::binary|std::ios::out);

          while(!std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size()).contains("\r\n--" + boundary_))
          {
            std::string_view buffer_scope(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size());
            std::string_view data_range;
            auto position = buffer_scope.rfind("\r\n--");
            if(position == std::string_view::npos || (buffer_scope.size() - position -1) >( boundary_.size() + 10))
              data_range = std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size());
            else
              data_range = buffer_scope.substr(0, buffer_scope.rfind("\r\n--"));
            stream.write(data_range.data(), data_range.size());
            streambuf_.consume(data_range.size());
            yield();
          }
          // Write Tail Data
          std::string_view tail_data(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size());
          std::string_view file_tail = tail_data.substr(0, tail_data.find("\r\n--" + boundary_));
          stream.write(file_tail.data(), file_tail.size());
          streambuf_.consume(file_tail.size() + 2);
          stream.flush();
          stream.close();
          continue;
        }else{
          while(!std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size()).contains("\r\n--" + boundary_))
          {
            yield();
          }
          std::string_view block_data(boost::asio::buffer_cast<const char*>(streambuf_.data()), streambuf_.size());
            sahara::string form_data(block_data.substr(0, block_data.find("\r\n--" + boundary_)));
          request_.request_params_.set(block_meta_data["name"].get_value_or(""), form_data);
          streambuf_.consume(form_data.size()+2);
          continue;
        }
      }
  }

    http_request_parser() = default;
  private:
    void on_request_impl(http::verb method, string_view method_str, string_view target, int version, error_code &ec) override{
      request_.method_ = method;
      request_.version_ = version;
        sahara::string string_params;
      auto split = target.find_first_of('?');
      if(split == std::string::npos)
        request_.target_path_ = target;
      else{
        request_.target_path_ = target.substr(0, split);
        string_params = target.substr(split+1, target.size()-1);
        parse_query_item(string_params);
      }
    };



    void parse_query_item(std::string_view data){
      std::vector<std::string> params;
      boost::algorithm::split(params, data, boost::is_any_of("&"), boost::token_compress_on);
      for(auto &param : params){
        auto split = param.find_first_of('=');
          sahara::string param_name, param_value;
        param_name = split == std::string::npos? param: param.substr(0, split);
        param_value =split == std::string::npos? "":param.substr(split+1, param.size()-1);
        param_value = sahara::coder::url_decode(param_value);
        request_.request_params_.set(param_name, param_value);
      }
    }


    void on_response_impl(int code, string_view reason, int version, boost::beast::error_code &ec) override{
      std::cout << "ResponseImpl:" << code << "," << reason << "," << version << std::endl;
    };

    void on_field_impl(http::field f, string_view name, string_view value, error_code &ec) override{
      if(f == http::field::content_type){
        std::string_view boundary_str = "boundary=";
        auto iter = boost::algorithm::ifind_first(value, boundary_str);
        if(iter)
          boundary_ = value.substr(std::distance(value.begin(), iter.begin()) + boundary_str.length());
      }
      request_.headers_.emplace(name, value);
    };

    void on_header_impl(error_code &ec) override{
      if(request_.headers_.contains("connection") && boost::iequals(request_.headers_["connection"],"keep-alive"))
        request_.keep_alive_ = true;
    };

    void on_body_init_impl(boost::optional<std::uint64_t> const &content_length, error_code &ec) override{
      remain_ = content_length.get_value_or(0);
      if(!request_.headers_.contains("Content-Type")) return;
      if(request_.headers_["Content-Type"].contains("multipart/form-data")){
        body_handle_routine_ = std::make_unique<boost::coroutines2::coroutine<void>::pull_type>(std::bind_front(&http_request_parser::form_data_body_parser, this));
      }else if(request_.headers_["Content-Type"].contains("application/x-www-form-urlencoded")){
        body_handle_routine_ = std::make_unique<boost::coroutines2::coroutine<void>::pull_type>(std::bind_front(&http_request_parser::url_encoded_body_parser, this));
      }
    };

    std::size_t on_body_impl(string_view s, error_code &ec) override{
      streambuf_.sputn(s.begin(), s.length());
      remain_ -= s.length();
      if(body_handle_routine_)
        body_handle_routine_->operator()();
      return s.length();
    };

    void on_chunk_header_impl(std::uint64_t size, string_view extension, error_code &ec) override{
      std::cout << "OnChunkHeaderImpl:" << size << "," << extension  << std::endl;
    };

    std::size_t on_chunk_body_impl(std::uint64_t remain, string_view body, error_code &ec) override{
      std::cout << "OnChunkBodyImpl:" << remain << "," << body  << std::endl;
      return 10;
    };

    void on_finish_impl(error_code &ec) override{};

  public:

    http_request& get(){ return request_; }
    http_request release(){ return std::move(request_);}

    //void content_handler(boost::coroutines2::coroutine<int>::push_type& yield, )
  private:
    bool start = false;
    http_request request_;
    std::uint64_t remain_;
      sahara::string boundary_;
    boost::asio::streambuf streambuf_;
    std::unique_ptr<std::thread> worker_;
    std::unique_ptr<boost::coroutines2::coroutine<void>::pull_type> body_handle_routine_;
  };

} // obelisk

#endif //OBELISK_HTTP_REQUEST_PARSER_H
