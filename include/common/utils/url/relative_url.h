//
// Created by Hackman.Lo on 3/16/2023.
//

#ifndef OBELISK_RELATIVE_URL_H
#define OBELISK_RELATIVE_URL_H

#include <string_view>
#include <vector>
#include "string/string.h"


namespace obelisk {
  namespace utils {

    struct relative_url {
      static std::vector<sahara::string> split(const sahara::string& url);
    };

  } // obelisk
} // utils

#endif //OBELISK_RELATIVE_URL_H
