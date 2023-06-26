#include "common/request/http_request_raw.h"
#include <boost/algorithm/string.hpp>
namespace obelisk {

    bool iequal::operator()(const sahara::string &s1, const sahara::string &s2) const {
        if (s1.size() != s2.size()) return false;
        return s1.iequals(s2);
    }

    std::size_t ihash::operator()(const sahara::string &key) const {
        auto lower = boost::to_lower_copy(key.to_std());
        return std::hash<std::string>()(lower);
    }
}