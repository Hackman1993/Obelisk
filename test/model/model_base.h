//
// Created by Hackman.Lo on 4/6/2023.
//

#ifndef OBELISK_MODEL_BASE_H
#define OBELISK_MODEL_BASE_H
#include <string>
#include <vector>
#include <unordered_map>
#include <rttr/registration>
#include "common\details\request_param_container.h"
#include "exception/validation_exception.h"
template <class UserModel>
class model_base {
public:
  static UserModel create(obelisk::request_param_container& data) {
    UserModel model;
    rttr::array_range<rttr::property> properties = rttr::type::get(model).get_properties();
    for(const rttr::property& property:properties){
      std::string property_name =property.get_name().to_string();
      if(data.contains(property_name))
      {
        if(type_match<int>(property))
          property.set_value(model, boost::lexical_cast<int>(data.get(property_name)));
        else if(type_match<double>(property))
          property.set_value(model, boost::lexical_cast<double>(data.get(property_name)));
        else if(type_match<long double>(property))
          property.set_value(model, boost::lexical_cast<long double>(data.get(property_name)));
        else if(type_match<float>(property))
          property.set_value(model, boost::lexical_cast<float>(data.get(property_name)));
        else if(type_match<std::string>(property))
          property.set_value(model, data.get(property_name));
        else throw obelisk::exception::validation_exception("Unknown type: " + std::string(property.get_type().get_name()));
      }
      std::cout << property_name << std::endl;
    }
    return model;
  }
protected:
  template <typename Convert>
  static bool type_match(const rttr::property& property) {
    return property.get_type() == rttr::type::get<Convert>();
  }

  std::string table_;
};


#endif //OBELISK_MODEL_BASE_H
