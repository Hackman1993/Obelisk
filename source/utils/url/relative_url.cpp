//
// Created by Hackman.Lo on 3/16/2023.
//

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "utils/url/relative_url.h"


namespace obelisk {
  namespace utils {
    std::vector <std::string> utils::relative_url::split(std::string_view url) {
      std::vector<std::string> split_path;
      boost::algorithm::split(split_path, url, boost::is_any_of("/"),boost::algorithm::token_compress_on);
      erase_if(split_path,[](std::string &s){ return s.empty();});
      return std::move(split_path);
    }
  } // obelisk
} // utils