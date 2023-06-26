//
// Created by Hackman.Lo on 4/27/2023.
//

#include "common/request/request_body_parser.h"

namespace obelisk {
  struct body_meta{
    std::string name_;
    bool is_file_;
    std::string file_name_;
  };
  body_meta parse_multipart_body_meta(std::string_view source, http_request_raw &request) {
    return {};
  }
} // obelisk