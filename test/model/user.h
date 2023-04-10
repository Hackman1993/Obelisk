//
// Created by Hackman.Lo on 4/6/2023.
//

#ifndef OBELISK_USER_H
#define OBELISK_USER_H
#include "model.h"
#include <chrono>
#include <variant>
#include <rttr/registration_friend>
namespace obelisk::orm{
  class user : public model<user, rosetta::string>{
  public:
    rosetta::string user_id = nullptr;
    rosetta::string username = nullptr;
    rosetta::string password = nullptr;
    rosetta::string email = nullptr;
    rosetta::timestamp email_verified_at = nullptr;
    rosetta::string real_name = nullptr;
    rosetta::boolean status = false;
    rosetta::string passport_no = nullptr;
    rosetta::string avatar_url = nullptr;
    rosetta::string phone_number = nullptr;
    rosetta::string organization_id = nullptr;
    rosetta::timestamp created_at = nullptr;
    rosetta::timestamp updated_at = nullptr;
    rosetta::timestamp deleted_at = nullptr;

    std::string_view table() override;
    std::string_view primary_key() override;
    rosetta::string generate_id() override;
  protected:
    std::vector<std::string> hidden_ = {"password"};
    std::string table_ = "t_user";
    RTTR_REGISTRATION_FRIEND
  };

  std::string_view user::table() {
    return "t_user";
  }

  std::string_view user::primary_key() {
    return "user_id";
  }
  rosetta::string user::generate_id() {
    return rosetta::string(boost::replace_all_copy(boost::uuids::to_string(boost::uuids::random_generator()()),"-",""));
  }
}

RTTR_REGISTRATION
{
  using namespace rttr;
  registration::class_<obelisk::orm::user>("model::user")
      .constructor<>()
      .method("create", &obelisk::orm::user::create)
      .method("table", &obelisk::orm::user::table)
      .method("primary_key", &obelisk::orm::user::primary_key)
      .property("username", &obelisk::orm::user::username)
      .property("password", &obelisk::orm::user::password)
      .property("organization_id", &obelisk::orm::user::organization_id)
      .property_readonly("table_", &obelisk::orm::user::table_)
      .property("hidden_", &obelisk::orm::user::hidden_)
      .property("user_id", &obelisk::orm::user::user_id)
      .property("email", &obelisk::orm::user::email)
      .property("email_verified_at", &obelisk::orm::user::email_verified_at)
      .property("real_name", &obelisk::orm::user::real_name)
      .property("status", &obelisk::orm::user::status)
      .property("passport_no", &obelisk::orm::user::passport_no)
      .property("avatar_url", &obelisk::orm::user::avatar_url)
      .property("phone_number", &obelisk::orm::user::phone_number)
      .property("created_at", &obelisk::orm::user::created_at)
      .property("updated_at", &obelisk::orm::user::updated_at)
      .property("deleted_at", &obelisk::orm::user::deleted_at);
}




#endif //OBELISK_USER_H
