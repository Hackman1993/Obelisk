//
// Created by Hackman.Lo on 3/21/2023.
//

#ifndef OBELISK_AUTH_CONTROLLER_H
#define OBELISK_AUTH_CONTROLLER_H

#include "controller/controller_base.h"
#include "request/string_response.h"
#include <iostream>
class auth_controller{
public:
  static std::unique_ptr<obelisk::http_response> test(obelisk::http_request &request){
    for(auto & rou: request.route_params()){
      std::cout << rou.first <<":" << rou.second << std::endl;
    }
    return std::make_unique<obelisk::string_response>(200, "HDKFHA");
  }
};


#endif //OBELISK_AUTH_CONTROLLER_H
