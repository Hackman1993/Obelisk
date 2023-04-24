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
    static rosetta::database_pool& make_pool(const sahara::string& name, std::size_t pool_size, const sahara::string& host, std::uint16_t port, sahara::string& username, sahara::string& password, sahara::string& database){
      pools_.emplace(name, std::make_unique<rosetta::database_pool>(pool_size, host, port, username, password, database));
      return *pools_[name];
    }

    template <typename Connection>
    static std::shared_ptr<Connection> get_connection(const sahara::string& name){
      if(pools_.contains(name)){
        return pools_[name]->template get_connection<Connection>();
      }
      return nullptr;
    }

  protected:
    static std::unordered_map<sahara::string, std::unique_ptr<rosetta::database_pool>> pools_;
  };

} // obelisk

#endif //OBELISK_DB_H
