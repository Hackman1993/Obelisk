//
// Created by Hackman.Lo on 3/22/2023.
//

#ifndef OBELISK_NOCASE_SUPPORT_H
#define OBELISK_NOCASE_SUPPORT_H
#include <boost/functional/hash.hpp>
#include <boost/algorithm/string.hpp>
namespace obelisk{
  struct no_case_equal{
    bool operator()(const std::string& s1, const std::string& s2) const{
      if(s1.size()!=s2.size()) return false;
      return boost::algorithm::iequals(s1, s2);
    }
  };

  struct no_case_hash{
    std::size_t operator()(const std::string& key) const{
      return boost::hash<std::string>()(boost::to_lower_copy(key));
    }
  };
}

#endif //OBELISK_NOCASE_SUPPORT_H
