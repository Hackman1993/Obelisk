//
// Created by Hackman.Lo on 4/6/2023.
//

#ifndef OBELISK_MODEL_H
#define OBELISK_MODEL_H
#include <string>
#include <vector>
#include "model_base.h"
#include <unordered_map>
#include <rttr/registration>
#include "common\details\request_param_container.h"
#include "exception/validation_exception.h"
namespace obelisk::orm{
  template <class UserModel, typename IndexType>
  class model : public model_base{
  public:
    static UserModel create(obelisk::request_param_container& data) {
      UserModel model;
      rttr::array_range<rttr::property> properties = rttr::type::get<UserModel>().get_properties();
      for(const rttr::property& property:properties){
        std::string property_name =property.get_name().to_string();
        if(data.contains(property_name))
        {
          if(type_match<rosetta::integer>(property))
            property.set_value(model, rosetta::integer(boost::lexical_cast<int>(data.get(property_name))));
          else if(type_match<rosetta::long_double>(property))
            property.set_value(model, rosetta::long_double(boost::lexical_cast<long double>(data.get(property_name))));
          else if(type_match<rosetta::boolean>(property))
            property.set_value(model, rosetta::boolean(boost::lexical_cast<bool>(data.get(property_name))));
          else if(type_match<rosetta::string>(property))
            property.set_value(model, rosetta::string(data.get(property_name)));
          else throw obelisk::exception::validation_exception("Unknown type: " + std::string(property.get_type().get_name()));
        }
        if(property_name == model.primary_key()){
          rosetta::string test = property.get_value(model).template get_value<IndexType>();
          if(test.is_null()){
            property.set_value(model, model.generate_id());
          }
        }
      }

      return model;
    }
  protected:
    template <typename Convert>
    static bool type_match(const rttr::property& property) {
      return property.get_type() == rttr::type::get<Convert>();
    }
  };
}
#endif //OBELISK_MODEL_H
