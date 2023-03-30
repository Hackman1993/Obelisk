//
// Created by Hackman.Lo on 3/28/2023.
//

#include "user_controller.h"
#include "DB.h"
#include "exception/http_exception.h"

namespace obelisk {

  std::unique_ptr<obelisk::http_response> user_controller::get_token(http_request &request) {
    request.validate({
       { "login", { validator::required() }},
       { "password", {validator::required(), validator::min_length(8)}}
    });
    std::string username = request.param("login");
    std::string password = request.param("password");
    auto conn = DB::get_connection<rosetta::mysql_connection>("default");
    if(!conn) throw exception::http_exception("No Database Connection Available", 200);
    auto stmt = conn->prepared_statement("select * from t_user where username is not null limit 10");
    //stmt.bind_param(1, username);
    //stmt.bind_param(2, password);
    auto result = stmt.execute();
    std::string password_encryption = result->get<rosetta::sql_string>(0, "created_at");
    auto type = result->column_type("created_at");
    type = result->column_type("password");


    return json_response(boost::polymorphic_pointer_cast<rosetta::sql_result>(result));
  }
} // obelisk