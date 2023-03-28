//
// Created by Hackman.Lo on 3/28/2023.
//

#ifndef OBELISK_DB_H
#define OBELISK_DB_H

#include <unordered_map>
#include "rosetta.h"
namespace obelisk {

  class DB {
  public:
    static rosetta::database_pool& make_pool(const std::string& name, std::size_t pool_size, std::string_view host, std::uint16_t port, std::string_view username, std::string_view password, std::string_view database){
      pools_.emplace(name, std::make_unique<rosetta::database_pool>(pool_size, host, port, username, password, database));
      return *pools_[name];
    }

  protected:
    static std::unordered_map<std::string, std::unique_ptr<rosetta::database_pool>> pools_;
  };

} // obelisk

#endif //OBELISK_DB_H
