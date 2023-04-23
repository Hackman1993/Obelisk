//
// Created by Hackman.Lo on 3/28/2023.
//

#ifndef OBELISK_USER_CONTROLLER_H
#define OBELISK_USER_CONTROLLER_H
#include "controller/controller_base.h"
#include "DB.h"
namespace obelisk {

  class user_controller :public controller_base {
  public:
    static std::unique_ptr<obelisk::http_response> get_token(obelisk::http_request &request);
    static std::unique_ptr<obelisk::http_response> current(obelisk::http_request &request);
    static std::unique_ptr<obelisk::http_response> create(obelisk::http_request &request);
    static std::unique_ptr<obelisk::http_response> update(obelisk::http_request &request);

  };

} // obelisk

#endif //OBELISK_USER_CONTROLLER_H
