//
// Created by Hackman.Lo on 4/6/2023.
//

#ifndef OBELISK_USER_H
#define OBELISK_USER_H
#include "model_base.h"
#include <rttr/registration_friend>
namespace model{
  class user : public model_base<user>{
  public:
    std::string username;
    std::string password;
    std::string organization_id;
  protected:
    std::vector<std::string> hidden_ = {"password"};
    std::string table_ = "t_user";
    RTTR_REGISTRATION_FRIEND
  };


}

RTTR_REGISTRATION
{
  using namespace rttr;
  registration::class_<model::user>("model::user")
      .constructor<>()
      .method("create", &model::user::create)
      .property("username", &model::user::username)
      .property("password", &model::user::password)
      .property("organization_id", &model::user::organization_id)
      .property_readonly("table_", &model::user::table_)
      .property("hidden_", &model::user::hidden_);
}

#endif //OBELISK_USER_H
