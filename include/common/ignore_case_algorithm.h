//
// Created by Hackman.Lo on 3/20/2023.
//

#ifndef OBELISK_IGNORE_CASE_ALGORITHM_H
#define OBELISK_IGNORE_CASE_ALGORITHM_H
#include <string>
#include <boost/algorithm/string.hpp>
namespace obelisk {

  struct nocase_string_map_equal_ {
    bool operator()(const std::string& s1, const std::string& s2) const{
      if(s1.size()!=s2.size()) return false;
      return boost::algorithm::iequals(s1, s2);
    }
  };


} // obelisk

#endif //OBELISK_IGNORE_CASE_ALGORITHM_H
