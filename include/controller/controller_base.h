//
// Created by Hackman.Lo on 3/21/2023.
//

#ifndef OBELISK_CONTROLLER_BASE_H
#define OBELISK_CONTROLLER_BASE_H

#include "../common/request/http_request.h"
#include "../common/response/string_response.h"
#include "../common/validator/validator.h"
#include <rosetta.h>
#include <boost/json.hpp>
namespace obelisk {

  class controller_base {

  protected:
    static std::unique_ptr<string_response> json_response(const boost::json::value& result, std::size_t code = 200, const sahara::string& message=""){
        auto str = sahara::string::static_format(R"({{ "code": {}, "message": "{}", "data": {} }})", code == 200? 0:code, message, boost::json::serialize(result));
//      std::stringstream sstream;
//      sstream << "{" << "\"code\":" << (code == 200? 0:code) << R"(,"message":")" << message << R"(","data":)" << boost::json::serialize(result) << "}";
      return std::make_unique<string_response>(200, str);
    }
    static std::unique_ptr<string_response> json_response(std::nullptr_t result, std::size_t code = 200, const sahara::string& message=""){
        auto str = sahara::string::static_format(R"({{ "code": {}, "message": "{}", "data": null }})", code == 200? 0:code, message);
//      std::stringstream sstream;
//      sstream << "{" << "\"code\":" << (code == 200? 0:code) << R"(,"message":")" << message << R"(","data":null})";
      return std::make_unique<string_response>(200, str);
    }

    static std::unique_ptr<string_response> json_response(const std::string& result, std::size_t code = 200, const sahara::string& message=""){
        auto str = sahara::string::static_format(R"({{ "code": {}, "message": "{}", "data": {} }})", code == 200? 0:code, message, result);
//      std::stringstream sstream;
//      sstream << "{" << "\"code\":" << (code == 200? 0:code) << R"(,"message":")" << message << R"(","data": )"<< result << "}";
      return std::make_unique<string_response>(200, str);
    }

    static std::unique_ptr<string_response> json_response(const boost::json::object& result, std::size_t code = 200, sahara::string message=""){
        auto str = sahara::string::static_format(R"({{ "code": {}, "message": "{}", "data": {} }})", code == 200? 0:code, message, boost::json::serialize(result));
//      std::stringstream sstream;
//      sstream << "{" << "\"code\":" << (code == 200? 0:code) << R"(,"message":")" << message << R"(","data":)" << boost::json::serialize(result) << "}";
      return std::make_unique<string_response>(200, str);
    }

    static std::unique_ptr<string_response> json_response(const boost::json::array& result, std::size_t code = 200, sahara::string message=""){
      auto str = sahara::string::static_format(R"({{ "code": {}, "message": "{}", "data": {} }})", code == 200? 0:code, message, boost::json::serialize(result));
//      sstream << "{" << "\"code\":" << (code == 200? 0:code) << R"(,"message":")" << message << R"(","data":)" << boost::json::serialize(result) << "}";
      return std::make_unique<string_response>(200, str);
    }

    static std::unique_ptr<string_response> json_response(const std::shared_ptr<rosetta::sql_result>& result, std::size_t code = 200, sahara::string message=""){

        auto str = sahara::string::static_format(R"({{ "code": {}, "message": "{}", "data": {} }})", code == 200? 0:code, message, result->to_json());
//      std::stringstream sstream;
//      sstream << "{" << "\"code\":" << (code == 200? 0:code) << R"(,"message":")" << message << R"(","data":)" << result->to_json() << "}";
//      sst
      return std::make_unique<string_response>(200, str);
    }
  };

} // obelisk

#endif //OBELISK_CONTROLLER_BASE_H
