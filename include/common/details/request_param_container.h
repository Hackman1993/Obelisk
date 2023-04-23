//
// Created by Hackman.Lo on 3/20/2023.
//

#ifndef OBELISK_REQUEST_PARAM_CONTAINER_H
#define OBELISK_REQUEST_PARAM_CONTAINER_H

#include <string>
#include <unordered_map>

namespace obelisk {

    class request_param_container {
    public:
        void set(const std::string &key, const std::string &value) {
            if (params_.contains(key) && key.ends_with("[]"))
                params_[key].emplace_back(value);
            else {
                std::vector<std::string> temp_{value};
                auto result = params_.emplace(key, std::move(temp_));
            }
        }

        bool contains(const std::string &name) {
            return params_.contains(name);
        }

        bool contains(std::string_view name) {
            return params_.contains(std::string(name));
        }

        std::string get(const std::string &name) {
            if (!name.contains(name) || params_[name].size() == 0) return {};

            return params_[name][0];
        }

        std::vector<std::string> &get_all(const std::string &name) {
            return params_[name];
        }

        request_param_container only(const std::vector<std::string> &fields) {
            request_param_container result;
            for (const auto &field: fields) {
                if (params_.contains(field))
                    result.params_.emplace(field, params_[field]);
            }
            return result;
        }

    protected:
        std::unordered_map<std::string, std::vector<std::string>> params_;
    };

} // obelisk

#endif //OBELISK_REQUEST_PARAM_CONTAINER_H
