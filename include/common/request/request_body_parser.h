//
// Created by Hackman.Lo on 4/27/2023.
//

#ifndef LIBRARYORGANIZED_REQUEST_BODY_PARSER_H
#define LIBRARYORGANIZED_REQUEST_BODY_PARSER_H

#include <string>
#include "http_request_raw.h"

namespace obelisk {

  class body_meta;
  body_meta parse_multipart_body_meta(std::string_view source, http_request_raw &request);

} // obelisk

#endif //LIBRARYORGANIZED_REQUEST_BODY_PARSER_H
