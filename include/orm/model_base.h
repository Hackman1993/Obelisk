//
// Created by Hackman.Lo on 4/10/2023.
//

#ifndef OBELISK_MODEL_BASE_H
#define OBELISK_MODEL_BASE_H
#include <string_view>
#include "types/nullable.h"
#include <rttr/registration>
namespace obelisk::orm {

  class model_base {
  protected:
    template<typename Convert>
    static bool type_match(const rttr::property &property) {
      return property.get_type() == rttr::type::get<Convert>();
    }
  };

} // model

#endif //OBELISK_MODEL_BASE_H
