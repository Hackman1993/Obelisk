//
// Created by Hackman.Lo on 3/21/2023.
//

#ifndef OBELISK_CONTROLLER_BASE_H
#define OBELISK_CONTROLLER_BASE_H

#include "../common/request/http_request.h"
#include "../common/response/string_response.h"
#include "../common/validator/validator.h"
#include <rosetta.h>

namespace obelisk {

  class controller_base {

  protected:
    static std::unique_ptr<string_response> json_response(std::shared_ptr<rosetta::sql_result> result, std::size_t code = 200, std::string message=""){
      std::stringstream sstream;
      sstream << "{" << "\"code\":" << (code == 200? 0:code) << ",\"message\":\"" << message << "\", \"data\":[";
      bool first_row = true;
      for(int i =0; i< result->count(); i++){
        bool first_column = true;

        if(first_row) first_row = false;
        else sstream << ",";

        for(int j = 0; j< result->column_count(); j++){
          if(first_column) sstream<< "{", first_column = false;
          else sstream << ",";

          sstream << "\"" << result->column_name(j) << "\":";
          std::string type = result->column_type(j);
          if(
              boost::iequals(type,"varchar")||
              boost::iequals(type,"char")||
              boost::iequals(type, "timestamp")||
              boost::iequals(type, "datetime")
          ) sstream << result->get<rosetta::sql_string>(i, j).json_value();
          else{
            std::cout << "Unhandled Sql Type:" << type << std::endl;
            sstream << "null";
          }
        }
        sstream << "}";
      }
      sstream << "]}";
//      sst
      return std::make_unique<string_response>(200, sstream.str());
    }
  };

} // obelisk

#endif //OBELISK_CONTROLLER_BASE_H
