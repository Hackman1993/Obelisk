//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_HTTP_PACKAGE_H
#define OBELISK_HTTP_PACKAGE_H
#include <unordered_map>

namespace obelisk {
  class http_package {

  protected:
    std::unordered_map<std::string, std::string> headers_;
  };
}


#endif //OBELISK_HTTP_PACKAGE_H
