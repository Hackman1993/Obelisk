//
// Created by Hackman.Lo on 4/12/2023.
//

#ifndef OBELISK_TYPE_MATCH_H
#define OBELISK_TYPE_MATCH_H

#include <rttr/registration>
namespace obelisk {
  namespace orm {

    template <typename Type>
    struct type_match {
      bool operator()(rttr::property property)
      {
        return property.get_type() == rttr::type::get<Type>();
      }
    };

  } // obelisk
} // orm

#endif //OBELISK_TYPE_MATCH_H
