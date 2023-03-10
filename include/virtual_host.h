//
// Created by Hackman.Lo on 3/10/2023.
//

#ifndef OBELISK_VIRTUAL_HOST_H
#define OBELISK_VIRTUAL_HOST_H

#include <string>
#include <unordered_map>
#include <boost/asio/ssl/context.hpp>
#include "virtual_host_meta.h"

namespace obelisk{
  class http_server;
  class session_detector;

  class virtual_host {

  public:
    virtual_host(){}
    virtual_host(std::string domain, unsigned short port): domains_{ {virtual_host_meta(domain,port), true} }{}

    void load_certificate(std::string certificate, std::string private_key);
  private:
    unsigned short port_ = 0;
    std::string webroot_;
    virtual_host *next_ = nullptr;
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12};
    std::unordered_map<virtual_host_meta, bool, virtual_host_meta> domains_;

    friend class http_server;
    friend class session_detector;
  };
}

#endif //OBELISK_VIRTUAL_HOST_H
