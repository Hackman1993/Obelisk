//
// Created by Hackman.Lo on 3/15/2023.
//

#ifndef OBELISK_HTTP_HEADER_H
#define OBELISK_HTTP_HEADER_H

#include <string>
#include <boost/algorithm/string.hpp>

namespace obelisk{
  struct header_key_hash_{
    std::size_t operator()(const std::string& key) const{
      std::size_t h = 0;
      for(char c: key){
        h = (h << 1) ^ std::tolower(c);
      }
      return h;
    }
  };

  struct header_key_iequal_ {
    bool operator()(const std::string& s1, const std::string& s2) const{
      if(s1.size()!=s2.size()) return false;
      return boost::algorithm::iequals(s1, s2);
    }
  };



  using http_header = std::unordered_map<std::string, std::string, header_key_hash_, header_key_iequal_>;
}
#endif //OBELISK_HTTP_HEADER_H
