/***
 * @file   http_header.h
 * @author Hackman.Lo
 * @date  2023-07-06
 * @description Simple redefinition of std::unordered_map<std::string, std::string>, but with no case sensitivity
 */

#ifndef OBELISK_HTTP_HEADER_H
#define OBELISK_HTTP_HEADER_H

#include <string>
#include "nocase_support.h"
namespace obelisk{
  using http_header = std::unordered_map<sahara::string, sahara::string, no_case_hash, no_case_equal>;
}
#endif //OBELISK_HTTP_HEADER_H
