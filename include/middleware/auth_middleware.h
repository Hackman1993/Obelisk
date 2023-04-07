//
// Created by Hackman.Lo on 3/17/2023.
//

#ifndef OBELISK_AUTH_MIDDLEWARE_H
#define OBELISK_AUTH_MIDDLEWARE_H
#include <memory>
#include "../DB.h"
#include "../common/request/http_request.h"
#include "../common/response/string_response.h"
#include "../exception/http_exception.h"
namespace obelisk {

  class auth_middleware {
  public:
    static std::unique_ptr<http_response> handle(http_request& request, std::string_view params){
      if(!request.header().contains("Authorization")) throw exception::http_exception("Access Denied!", 403);

      const std::string token_header = "bearer ";
      std::string authorization_header = request.header()["Authorization"];
      auto token_itr = boost::ifind_first(authorization_header, token_header);
      if(!token_itr)  throw exception::http_exception("Access Denied!", 403);

      std::string token = authorization_header.substr(token_itr.size());
      auto connection = DB::get_connection<rosetta::mysql_connection>("default");
      auto stmt = connection->prepared_statement("select user_id from t_access_token where access_token_id = ?");

      stmt->bind_param(0, token);
      auto result = stmt->execute();
      if(result->count() <=0 )  throw exception::http_exception("Access Denied!", 403);
      request.params().set("__current_user_id", result->get<rosetta::sql_string>(0, "user_id"));

      return nullptr;
    }

  };

} // obelisk

#endif //OBELISK_AUTH_MIDDLEWARE_H
