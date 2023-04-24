//
// Created by Hackman.Lo on 3/15/2023.
//

#ifndef OBELISK_HTTP_HEADER_H
#define OBELISK_HTTP_HEADER_H

#include <string>
#include <boost/algorithm/string.hpp>
#include "nocase_support.h"
namespace obelisk{
  using http_header = std::unordered_map<sahara::string, sahara::string, no_case_hash, no_case_equal>;
}
#endif //OBELISK_HTTP_HEADER_H
