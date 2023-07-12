//
// Created by Hackman.Lo on 4/26/2023.
//

#ifndef LIBRARYORGANIZED_HTTP_REQUEST_RAW_H
#define LIBRARYORGANIZED_HTTP_REQUEST_RAW_H

#include <string>
#include <unordered_map>
#include <sahara/string/string.h>
#include "common/details/request_file.h"
#include "common/details/request_param_container.h"
#include "common/details/http_header.h"
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>


namespace obelisk {
    struct http_request_header_begin_io {
        std::string method_;
        std::string target_;
        std::string protocol_;
    };

    struct http_pair_data_io {
        std::string key_;
        std::string value_;
    };

    struct http_package_header_io {
        http_request_header_begin_io mat_;
        std::vector<http_pair_data_io> headers_;
    };
} // obelisk

#endif //LIBRARYORGANIZED_HTTP_REQUEST_RAW_H
