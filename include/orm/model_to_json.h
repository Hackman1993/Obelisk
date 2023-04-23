//
// Created by Hackman.Lo on 4/14/2023.
//

#ifndef OBELISK_MODEL_TO_JSON_H
#define OBELISK_MODEL_TO_JSON_H

#include <string>
#include "type_match.h"
#include <rttr/instance.h>
#include <common/sql_types.h>
#include <iostream>
#include "exception/validation_exception.h"
namespace obelisk::orm {

  template <typename UserModel>
  class model_to_json {

  public:
    virtual std::string to_json() {
      rttr::instance model((UserModel*)this);
      auto type = rttr::type::get<UserModel>();
      std::string json = "{";
      bool first = true;
      for (auto& property : type.get_properties()) {
        rosetta::timestamp tm;
        if(type_match<rosetta::timestamp>()(property)){
          if(first) first = false;
          else json.append(",");
          json.append("\"" + property.get_name().to_string() + "\":");
          json.append(property.get_value(model).get_value<rosetta::timestamp>().json());
        }
        else if(type_match<rosetta::integer>()(property)){
          if(first) first = false;
          else json.append(",");
          json.append("\"" + property.get_name().to_string() + "\":");
          json.append(property.get_value(model).get_value<rosetta::integer>().json());
        }
        else if(type_match<rosetta::long_double>()(property)){
          if(first) first = false;
          else json.append(",");
          json.append("\"" + property.get_name().to_string() + "\":");
          json.append(property.get_value(model).get_value<rosetta::long_double>().json());
        }
        else if(type_match<rosetta::boolean>()(property)){
          if(first) first = false;
          else json.append(",");
          json.append("\"" + property.get_name().to_string() + "\":");
          json.append(property.get_value(model).get_value<rosetta::boolean>().json());
        }
        else if(type_match<rosetta::string>()(property)){
          if(first) first = false;
          else json.append(",");
          json.append("\"" + property.get_name().to_string() + "\":");
          json.append(property.get_value(model).get_value<rosetta::string>().json());
        }
        else if(type_match<rosetta::unsigned_integer>()(property)){
          if(first) first = false;
          else json.append(",");
          json.append("\"" + property.get_name().to_string() + "\":");
          json.append(property.get_value(model).get_value<rosetta::unsigned_integer>().json());
        }
        else {
          std::cout << "Unknown type: " << std::string(property.get_type().get_name()) << std::endl;
          continue;
        }
      }
      json.append("}");
      return json;
    }

  };
} // obelisk::orm

#endif //OBELISK_MODEL_TO_JSON_H
