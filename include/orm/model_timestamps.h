//
// Created by Hackman.Lo on 4/12/2023.
//

#ifndef OBELISK_MODEL_TIMESTAMPS_H
#define OBELISK_MODEL_TIMESTAMPS_H
#include <rttr/registration>
#include "common/sql_types.h"
namespace obelisk::orm {

  template <typename UserModel>
  class model_timestamps {
  protected:
    void auto_set_created_at_() {
      auto* ptr = (UserModel*)this;
      rttr::instance model(*ptr);
      auto property = rttr::type::get<UserModel>().get_property("created_at");
      if (property.is_valid()) {
        property.set_value(model, rosetta::timestamp(sahara::time::timestamp()));
      }
    }

    void auto_set_updated_at_() {
      auto* ptr = (UserModel*)this;
      rttr::instance model(*ptr);
      auto property = rttr::type::get<UserModel>().get_property("updated_at");
      if (property.is_valid()) {
        property.set_value(model, rosetta::timestamp(sahara::time::timestamp()));
      }
    }
  };
}


#endif //OBELISK_MODEL_TIMESTAMPS_H
