//
// Created by Hackman.Lo on 3/20/2023.
//

#ifndef OBELISK_HTTP_REQUEST_PARSER_H
#define OBELISK_HTTP_REQUEST_PARSER_H

#include <boost/beast.hpp>
#include <iostream>
#include "http_request.h"
#include "common/request_param.h"
#include "common/request_file.h"
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace obelisk {
  using namespace boost::beast;
  using namespace boost::spirit::x3;
  using namespace std::chrono_literals;

  class http_request_parser : public http::basic_parser<true> {
  private:
    void form_data_body_parse_worker(){
      while(remain_!=0 || streambuf_.size()!=0){
        std::string_view unhandled;
        // Receiving First boundary
        while(!unhandled.contains("\r\n")) {
          std::this_thread::sleep_for(0.05s);
          unhandled = std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()),streambuf_.size());
        }
        std::string_view parsed_data(boost::asio::buffer_cast<const char*>(streambuf_.data()), unhandled.find("\r\n"));

        // If it's end boundary, exit thread
        if(parsed_data.ends_with(boundary_+"--")) return;
        streambuf_.consume(parsed_data.size()+2);

        // Reading Form Meta Data
        std::unordered_map<std::string, std::string> meta_data;
        for(bool ended = false;!ended;){
          while(!unhandled.contains("\r\n")) {
            std::this_thread::sleep_for(0.05s);
            unhandled = std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()),streambuf_.size());
          }
          if(unhandled.starts_with("\r\n"))
          {
            streambuf_.consume(2);
            break;
          }
          parsed_data = std::string_view(boost::asio::buffer_cast<const char*>(streambuf_.data()), unhandled.find("\r\n"));
          std::string_view header = parsed_data.substr(0, parsed_data.find(':'));
          std::string_view value = parsed_data.substr(parsed_data.find(':')+1, parsed_data.size()-1);
          meta_data.emplace(header, value);
          streambuf_.consume(parsed_data.length()+2);
        }

        // check if it's a file
        if(meta_data.contains("filename"))
        {

        }else{

        }

        std::unordered_map<std::string, boost::optional<std::string>> block_meta_data;
        auto parser = (+~char_(";=") >> -(lit("=") >> '\"' >> +~char_("\"") >> "\""))%";";
        bool parse_result  = boost::spirit::x3::phrase_parse(value.begin(),value.end(), parser, ascii::space, block_meta_data);
        if(!parse_result)
          std::cout << "Parse_failed:" << value << std::endl;

      }
    }
    void on_request_impl(http::verb method, string_view method_str, string_view target, int version, error_code &ec) override{
      request_.method_ = method;
      request_.version_ = version;


      std::string string_params;

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
        std::string param_name, param_value;
        param_name = split == std::string::npos? param: param.substr(0, split);
        param_value =split == std::string::npos? "":param.substr(split+1, param.size()-1);
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

    void on_header_impl(error_code &ec) override{};

    void on_body_init_impl(boost::optional<std::uint64_t> const &content_length, error_code &ec) override{
      remain_ = content_length.get_value_or(0);
    };

    std::size_t on_body_impl(string_view s, error_code &ec) override{
      streambuf_.sputn(s.begin(), s.length());
      remains_ -= s.length();
//      if(!boundary_.empty()){
//          std::string_view parsed_data(boost::asio::buffer_cast<const char*>(streambuf_.data()), unhandled.find("\r\n"));
//          if(body_parse_state_ == 0 && parsed_data.ends_with(boundary_)){
//              streambuf_.consume(parsed_data.size() +2);
//              body_parse_state_ ++;
//          }else if(body_parse_state_ == 1){
//            std::string_view header = parsed_data.substr(0, parsed_data.find(':'));
//            std::string_view value = parsed_data.substr(parsed_data.find(':')+1, parsed_data.size()-1);
//            if(boost::iequals(header, "Content-Disposition")){
//              std::unordered_map<std::string, boost::optional<std::string>> result;
//              auto parser = (+~char_(";=") >> -(lit("=") >> '\"' >> +~char_("\"") >> "\""))%";";
//              bool parse_result  = boost::spirit::x3::phrase_parse(value.begin(),value.end(), parser, ascii::space, result);
//              if(!parse_result)
//                std::cout << "Parse Failed:" << parsed_data << std::endl;
//              if(result.contains("filename")){
//                boost::uuids::uuid uuid = boost::uuids::random_generator()();
//                //std::filesystem::
////                request_.file_bag_.emplace({
////
////                })
//                current_file_.temp_file_path = "./" + boost::uuids::to_string(uuid);
//                current_file_.original_name = result["filename"];
//                current_file_.stream.open(current_file_.stream);
//                //current_file_.n
//              }
//            }
//          }
//        }
//      }
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
    http_request_parser() = default;

    http_request& get(){ return request_; }
    http_request release(){ return std::move(request_);}
  private:
    bool start = false;
    http_request request_;
    std::uint64_t remain_;
    std::string boundary_;
    boost::asio::streambuf streambuf_;
    std::unique_ptr<std::thread> worker_;
  };

} // obelisk

#endif //OBELISK_HTTP_REQUEST_PARSER_H
