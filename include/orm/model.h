//
// Created by Hackman.Lo on 4/3/2023.
//

#ifndef OBELISK_MODEL_H
#define OBELISK_MODEL_H
#include <any>
#include <unordered_map>
namespace obelisk {
  struct field_describer{

  };

  class model {

  protected:
    std::unordered_map<std::string, std::any> values_;
    static std::unordered_map<std::string, field_describer> describers_;
  };

} // obelisk

#endif //OBELISK_MODEL_H
