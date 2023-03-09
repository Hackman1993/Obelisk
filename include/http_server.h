//
// Created by Hackman.Lo on 3/9/2023.
//

#ifndef OBELISK_HTTP_SERVER_H
#define OBELISK_HTTP_SERVER_H


#include <string>
namespace obelisk{
  class http_server {
  public:
    http_server(const std::string& address, unsigned short port, const std::string& webroot, unsigned int thread);
    void listen(const std::string& address, unsigned short port, const std::string& webroot, unsigned int thread);

  };
}


#endif //OBELISK_HTTP_SERVER_H
