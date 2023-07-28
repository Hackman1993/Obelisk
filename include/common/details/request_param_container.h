//
// Created by Hackman.Lo on 3/20/2023.
//

#ifndef OBELISK_REQUEST_PARAM_CONTAINER_H
#define OBELISK_REQUEST_PARAM_CONTAINER_H

#include <string>
#include <unordered_map>
#include <rosetta/rosetta.h>
namespace obelisk {

    class request_param_container {
    public:
        void set(const sahara::string &key, const rosetta::core::sql_param_value &value) {
            if (params_.contains(key) && key.ends_with("[]"))
                params_[key].emplace_back(value);
            else {
                std::vector<rosetta::core::sql_param_value> temp_{value};
                auto result = params_.emplace(key, std::move(temp_));
            }
        }

        bool contains(const sahara::string &name) {
            return params_.contains(name);
        }

        rosetta::core::sql_param_value get(const sahara::string &name) {
            if (!name.contains(name) || params_[name].size() == 0) return {};

            return params_[name][0];
        }

        std::vector<rosetta::core::sql_param_value> &get_all(const sahara::string &name) {
            return params_[name];
        }

        request_param_container only(const std::vector<sahara::string> &fields) {
            request_param_container result;
            for (const auto &field: fields) {
                if (params_.contains(field))
                    result.params_.emplace(field, params_[field]);
            }
            return result;
        }

    protected:
        std::unordered_map<sahara::string, std::vector<rosetta::core::sql_param_value>> params_;
    };

} // obelisk

#endif //OBELISK_REQUEST_PARAM_CONTAINER_H
