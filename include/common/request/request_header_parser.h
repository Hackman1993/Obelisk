//
// Created by Hackman.Lo on 4/26/2023.
//

#ifndef LIBRARYORGANIZED_REQUEST_HEADER_PARSER_H
#define LIBRARYORGANIZED_REQUEST_HEADER_PARSER_H
#include <string>
#include "http_request_raw.h"
namespace obelisk {
  bool parse_request_header(std::string_view source, http_request_raw &request);
} // obelisk

#endif //LIBRARYORGANIZED_REQUEST_HEADER_PARSER_H
