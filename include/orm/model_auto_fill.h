//
// Created by Hackman.Lo on 4/12/2023.
//

#ifndef OBELISK_MODEL_AUTO_FILL_H
#define OBELISK_MODEL_AUTO_FILL_H

#include "type_match.h"
#include "exception/validation_exception.h"
#include <common/sql_types.h>
#include <boost/lexical_cast.hpp>
#include "common/details/request_param_container.h"
namespace obelisk::orm {

    template <typename UserModel>
    class model_auto_fill {
    protected:
      virtual void set_field_(obelisk::request_param_container &data){
        rttr::instance model((UserModel*)this);
        auto properties = rttr::type::get<UserModel>().get_properties();
        bool dirty = false;
        for (const rttr::property &property: properties) {
          std::string property_name = property.get_name().to_string();
          if (data.contains(property_name)) {
            if (type_match<rosetta::integer>()(property)) {
              auto value = rosetta::integer(boost::lexical_cast<int64_t>(data.get(property_name)));
              property.set_value(model, value);
            } else if (type_match<rosetta::long_double>()(property)) {
              auto value = rosetta::long_double(boost::lexical_cast<long double>(data.get(property_name)));
              property.set_value(model, value);
            } else if (type_match<rosetta::boolean>()(property)) {
              auto value = rosetta::boolean(boost::lexical_cast<bool>(data.get(property_name)));
              property.set_value(model, value);
            } else if (type_match<rosetta::string>()(property)) {
              auto value = rosetta::string(data.get(property_name));
              property.set_value(model, value);
            } else throw obelisk::exception::validation_exception("Unknown type: " + std::string(property.get_type().get_name()));
            dirty = true;
          }
        }
      }
    };
} // obelisk::orm

#endif //OBELISK_MODEL_AUTO_FILL_H
