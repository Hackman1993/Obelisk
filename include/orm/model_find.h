#ifndef OBELISK_MODEL_FIND_H
#define OBELISK_MODEL_FIND_H

#include <common/sql_types.h>
template <typename UserModel>
concept Model = requires {
    { UserModel::table() } -> std::same_as<std::string_view>;
    { UserModel::primary_key() } -> std::same_as<std::string_view>;
};
namespace obelisk {
    namespace orm {

        template <typename UserModel>
        requires Model<UserModel>
        class model_find {
            sahara::types::nullable<UserModel> find_by_id(rosetta::string id) {
//                UserModel model;
//                sahara::string sql = "SELECT * FROM " + table() + " WHERE " + primary_key() + " = " + id;
            }
        };

    } // obelisk
} // orm

#endif //OBELISK_MODEL_FIND_H
