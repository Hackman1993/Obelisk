//
// Created by Hackman.Lo on 4/26/2023.
//

#ifndef LIBRARYORGANIZED_HTTP_REQUEST_RAW_H
#define LIBRARYORGANIZED_HTTP_REQUEST_RAW_H

#include <string>
#include <unordered_map>
#include "string/string.h"
#include "common/details/request_file.h"
#include "common/details/request_param_container.h"
#include "common/details/http_header.h"

namespace obelisk {
  struct iequal {
    bool operator()(const sahara::string &s1, const sahara::string &s2) const;
  };

  struct ihash {
    std::size_t operator()(const sahara::string &key) const;
  };

  struct http_request_raw {
    sahara::string method_;
    sahara::string path_;
    sahara::string version_;
    sahara::string boundary_;
    sahara::string host_;
    sahara::string accept_;
    sahara::string target_;
    bool keep_alive_: 1;
    std::size_t content_length_;
    std::size_t bytes_remains_;
    std::size_t maximum_body_size_ = 2048000;
    http_header headers_;
    std::unordered_map<sahara::string, request_file> file_bag_;
    request_param_container request_params_;
  };

} // obelisk

#endif //LIBRARYORGANIZED_HTTP_REQUEST_RAW_H
