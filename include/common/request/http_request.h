//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_HTTP_REQUEST_H
#define OBELISK_HTTP_REQUEST_H

#include "../details/request_file.h"
#include "../details/request_param_container.h"
#include "../details/nocase_support.h"
#include "../details/http_header.h"
#include <initializer_list>
#include "../validator/validator_base.h"
#include "http_request_raw.h"

namespace obelisk {

    class http_session;
    class http_request {
    public:
        void validate(const std::vector<validator::validator_group>& validators) {
            for (auto &i: validators) {
                for (auto &j: i.validators_) {
                    j->validate(i.name_, *this);
                }
            }
        }

        const sahara::string &method() const {
            return method_;
        }

        const sahara::string &path() const {
            return path_;
        }

        std::unordered_map<sahara::string, sahara::string> &route_params() {
            return route_params_;
        }

        std::unordered_map<sahara::string, sahara::string> &header() {
            return headers_;
        }

        request_param_container &params() {
            return request_params_;
        }

        rosetta::core::sql_param_value param(const sahara::string &name) {
            return request_params_.get(name);
        }

        std::vector<rosetta::core::sql_param_value> paramv(const sahara::string &name) {
            return request_params_.get_all(name);
        }

        std::size_t content_length()
        {
            return content_length_;
        }

        http_request() = default;

    protected:
        sahara::string method_;
        sahara::string target_;
        sahara::string path_;
        sahara::string protocol_;
        std::size_t content_length_ = 0;
        std::streamsize bytes_remain_ = 0;
        request_param_container request_params_;
        std::unordered_map<sahara::string, request_file> file_bag_;
        std::unordered_map<sahara::string, sahara::string> headers_;
        std::unordered_map<sahara::string, sahara::string> route_params_;

        friend class route_item;
        friend class http_session;
    };
}


#endif //OBELISK_HTTP_REQUEST_H
