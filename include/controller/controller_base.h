//
// Created by Hackman.Lo on 3/21/2023.
//

#ifndef OBELISK_CONTROLLER_BASE_H
#define OBELISK_CONTROLLER_BASE_H

#include "../common/request/http_request.h"
#include "../common/response/json_response.h"
#include "../common/validator/validator.h"
#include <boost/json.hpp>
namespace obelisk {

  class controller_base {

  protected:
    static std::shared_ptr<http_response> json_response(const boost::json::value& result, std::size_t code = 200, const sahara::string& message=""){
      auto data = boost::json::object{
          {"code", code == 200? 0:code},
          {"message", message.to_std()},
          {"data", result}
      };
      return std::make_unique<obelisk::json_response>(data, code);
    }

  };

} // obelisk

#endif //OBELISK_CONTROLLER_BASE_H
