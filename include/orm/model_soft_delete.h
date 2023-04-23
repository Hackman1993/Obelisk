//
// Created by Hackman.Lo on 4/12/2023.
//

#ifndef OBELISK_MODEL_SOFT_DELETE_H
#define OBELISK_MODEL_SOFT_DELETE_H
#include <rttr/registration>
#include "common/sql_types.h"
namespace obelisk::orm {
    template <typename UserModel>
    class model_soft_delete {
    protected:
      void auto_set_deleted_at_() {
        auto property = rttr::type::get<UserModel>().get_property("deleted_at");
        if (property.is_valid()) {
          property.set_value(*this, rosetta::timestamp(sahara::time::timestamp()));
        }
      }

    };
} // obelisk::orm

#endif //OBELISK_MODEL_SOFT_DELETE_H
