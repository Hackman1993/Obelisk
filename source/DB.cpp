//
// Created by Hackman.Lo on 3/29/2023.
//
#include "DB.h"
namespace obelisk {
  std::unordered_map<std::string, std::unique_ptr<rosetta::database_pool>> DB::pools_;
}