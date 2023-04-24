//
// Created by Hackman.Lo on 3/21/2023.
//

#ifndef OBELISK_AUTH_CONTROLLER_H
#define OBELISK_AUTH_CONTROLLER_H

#include "controller/controller_base.h"
#include "common/response/string_response.h"
#include <iostream>
#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
class auth_controller{
public:
  static std::unique_ptr<obelisk::http_response> test(obelisk::http_request &request){

  }
};


#endif //OBELISK_AUTH_CONTROLLER_H
