//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_VIRTUAL_HOST_META_H
#define OBELISK_VIRTUAL_HOST_META_H
#include <string>

#include <boost/algorithm/string.hpp>
namespace obelisk {

  struct virtual_host_meta {
    virtual_host_meta() = default;
    virtual_host_meta(const std::string& domain, unsigned short port) :domain_(boost::to_lower_copy(domain)), port_(port){};


    bool operator == (const virtual_host_meta& other) const{
      return (domain_ == other.domain_ && port_ == other.port_);
    }

    std::size_t operator()(const virtual_host_meta& s) const{
      std::size_t port_hash = std::hash<unsigned short>()(s.port_);
      std::size_t domain_hash = std::hash<std::string>()(s.domain_);
      return domain_hash ^ (port_hash << 1);
    }
    std::string domain_;
    unsigned short port_ = 0;
  };
}


#endif //OBELISK_VIRTUAL_HOST_META_H
