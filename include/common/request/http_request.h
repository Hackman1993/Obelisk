//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_HTTP_REQUEST_H
#define OBELISK_HTTP_REQUEST_H

#include "common/details/request_file.h"
#include "common/details/request_param_container.h"
#include "common/details/nocase_support.h"
#include "common/details/http_header.h"
#include <initializer_list>
#include "../validator/validator_base.h"
#include "http_request_raw.h"

namespace obelisk {
  class http_request {
  public:
    http_request(http_request_raw raw) : raw_(std::move(raw)) {}
    void validate(std::vector<validator::validator_group> validators) {
      for (auto &i: validators) {
        for (auto &j: i.validators_) {
          j->validate(i.name_, *this);
        }
      }
    }

    const sahara::string& method() const {
      return raw_.method_;
    }

    const sahara::string &target_path() const {
      return raw_.target_;
    }

    std::unordered_map<sahara::string, sahara::string> &route_params() {
      return route_params_;
    }

    http_header &header() {
      return raw_.headers_;
    }

    request_param_container &params() {
      return raw_.request_params_;
    }

    sahara::string param(const sahara::string &name) {
      return raw_.request_params_.get(name);
    }

    std::vector<sahara::string> paramv(const sahara::string &name) {
      return raw_.request_params_.get_all(name);
    }

    http_request() = default;

  protected:
    http_request_raw raw_;
    std::unordered_map<sahara::string, sahara::string> route_params_;

    friend class route_item;

    friend class http_request_parser;
  };
}


#endif //OBELISK_HTTP_REQUEST_H
