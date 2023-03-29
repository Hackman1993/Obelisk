//
// Created by Hackman.Lo on 3/20/2023.
//

#ifndef OBELISK_REQUEST_PARAM_CONTAINER_H
#define OBELISK_REQUEST_PARAM_CONTAINER_H

#include <string>
#include <unordered_map>

namespace obelisk {

  class request_param_container {
  public:
    void set(const std::string& key, const std::string& value){
      if(params_.contains(key) && key.ends_with("[]"))
        params_[key].emplace_back(value);
      else{
        std::vector<std::string> temp_{value};
        auto result =  params_.emplace(key, std::move(temp_));
      }
    }

    bool contains(const std::string& name){
      return params_.contains(name);
    }
  protected:
    std::unordered_map<std::string, std::vector<std::string>> params_;
  };

} // obelisk

#endif //OBELISK_REQUEST_PARAM_CONTAINER_H
