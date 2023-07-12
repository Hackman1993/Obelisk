/**
 * @file  nocase_support.h
 */

#ifndef OBELISK_NOCASE_SUPPORT_H
#define OBELISK_NOCASE_SUPPORT_H
#include <boost/functional/hash.hpp>
#include <boost/algorithm/string.hpp>
#include <sahara/string/string.h>
namespace obelisk{
  struct no_case_equal{
    bool operator()(const sahara::string& s1, const sahara::string& s2) const{
      if(s1.size()!=s2.size()) return false;
      return s1.iequals(s2);
    }
  };

  struct no_case_hash{
    std::size_t operator()(const sahara::string& key) const{
      return boost::hash<std::string>()(boost::to_lower_copy(key.to_std()));
    }
  };
}

#endif //OBELISK_NOCASE_SUPPORT_H
