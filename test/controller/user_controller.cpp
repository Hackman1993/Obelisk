//
// Created by Hackman.Lo on 3/28/2023.
//

#include "user_controller.h"
#include "exception/http_exception.h"
#include "cryption/scrypt_hasher.h"
#include "exception/validation_exception.h"
#include "../database.h"
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/format.hpp>
#include <rosetta/common/sql/support/select.h>
#include <rosetta/mysql_impl/mysql_connection.h>
#include <rosetta/common/sql_row.h>
#include <cryption/bcrypt_hasher.h>

namespace obelisk {
    std::unique_ptr<obelisk::http_response> user_controller::get_token(http_request &request) {
        request.validate({{"login",    {validator::required()}},
                          {"password", {validator::required(), validator::min_length(6)}}});
        sahara::string username = request.param("login");
        sahara::string password = request.param("password");

        rosetta::core::select select_builder({"user_id", "username", "password", "token"});
        auto te = select_builder.from({{"t_user", ""}}).where("username", "=", "?").where("deleted_at", "is", "null").left_join(rosetta::core::alia{"t_user_access_token", "b"},[&](auto &join){
            join.on("user_id", "=", "b.fn_user_id");
        });
        auto connection = g_pool.get_connection<rosetta::mysql_connection>();
        if(!connection) throw exception::validation_exception("Failed to get connection!");
        auto s = select_builder.compile();

        auto statement = connection->prepared_statement(select_builder.compile());
        if(!statement) throw exception::validation_exception("Create Statement Failed");
        statement->bind_param(username.to_std());
        statement->execute();
        auto result = statement->get();
        if (result->count() <= 0) throw exception::validation_exception("Invalid Credential");
        auto row = result->next();
        if (!row) throw exception::validation_exception("Invalid Credential");

        std::uint64_t uid = std::get<std::uint64_t>(row->get_column(0));
        std::string uname = std::get<std::string>(row->get_column(1));
        std::string pwd = std::get<std::string>(row->get_column(2));
        rosetta::core::sql_param_value vtoken = row->get_column(3);

        bool check = obelisk::cryption::bcrypt::validate(password, pwd);
        if (!check) throw exception::validation_exception("Invalid Credential");
        std::string token;
        if(vtoken.index() == 0){
            token = boost::uuids::to_string(boost::uuids::random_generator()());
            statement = connection->prepared_statement("INSERT INTO t_user_access_token(fn_user_id, token) VALUES (?, ?)");
            if(statement) throw exception::validation_exception("Create Statement Failed");
            statement->bind_param(uid);
            statement->bind_param(token);
            statement->execute();
        }else{
        }

        return json_response(boost::json::object({{"user",        {{"user_id", uid}, {"username", uname}}},
                                                  {"token",       token}}));

    }

//    std::unique_ptr<obelisk::http_response> user_controller::current(http_request &request) {
//        sahara::string username = request.param("login");
//        auto conn = DB::get_connection<rosetta::mysql_connection>("default");
//        if (!conn) throw exception::http_exception("No Database Connection Available", 200);
//        auto stmt = conn->prepared_statement(
//                "select user_id,username,email,real_name,phone_number from t_user where user_id = ? and deleted_at is null and status=1");
//        auto test = request.param("__current_user_id");
//        stmt->bind_param(0, request.param("__current_user_id"));
//        auto result = stmt->execute();
//        if (!result || result->count() <= 0) return json_response(nullptr);
//        return json_response(result->get_row(0)->to_json());
//    }
//
//    std::unique_ptr<obelisk::http_response> user_controller::create(http_request &request) {
//        using namespace validator;
//        request.validate({
//                                 {"username",        {required(), unique("t_user", "username")}},
//                                 {"password",        {required(), min_length(8), confirmed()}},
//                                 {"real_name",       {required(), min_length(1)}},
//                                 {"organization_id", {required(), exists("t_organization", "organization_id")}},
//                         });
//
//        return nullptr;
//    }
//
//    std::unique_ptr<obelisk::http_response> user_controller::update(http_request &request) {
//        using namespace validator;
//        request.validate({
//            {"real_name",       {required(), min_length(1)}},
//            {"organization_id", {required(), exists("t_organization", "organization_id")}}
//        });
//
//        return nullptr;
//    }


}