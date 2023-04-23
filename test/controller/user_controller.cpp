//
// Created by Hackman.Lo on 3/28/2023.
//

#include "user_controller.h"
#include "DB.h"
#include "exception/http_exception.h"
#include "cryption/scrypt_hasher.h"
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/format.hpp>
#include "../model/user.h"

namespace obelisk {

    std::unique_ptr<obelisk::http_response> user_controller::get_token(http_request &request) {
        request.validate({
                                 {"login",    {validator::required()}},
                                 {"password", {validator::required(), validator::min_length(8)}}
                         });
        std::string username = request.param("login");
        std::string password = request.param("password");
        auto conn = DB::get_connection<rosetta::mysql_connection>("default");
        if (!conn) throw exception::http_exception("No Database Connection Available", 200);
        auto stmt = conn->prepared_statement("select password, user_id from t_user where username = ?");
        stmt->bind_param(0, request.param("login"));
        auto result = stmt->execute();
        if(result->count() <= 0) throw exception::validation_exception("Invalid Credential");

        std::string password_hash = result->get<rosetta::string>(0, "password");
        std::string user_id = result->get<rosetta::string>(0, "user_id");

        if (result && result->count()) {
            if (!obelisk::scrypt_hasher::check(password_hash, password))
                if(result->count() <= 0) throw exception::validation_exception("Invalid Credential", 200);
            auto uuid = boost::uuids::to_string(boost::uuids::random_generator()());
            boost::replace_all(uuid, "-", "");
            auto token_insert_stmt = conn->prepared_statement(
                    "insert into t_access_token(access_token_id, user_id) values(?,?)");
            token_insert_stmt->bind_param(0, uuid);
            token_insert_stmt->bind_param(1, user_id);
            token_insert_stmt->execute();
            return json_response(boost::json::object({{"access_token", uuid}}));
        }

        return json_response(nullptr);
    }

    std::unique_ptr<obelisk::http_response> user_controller::current(http_request &request) {
        std::string username = request.param("login");
        auto conn = DB::get_connection<rosetta::mysql_connection>("default");
        if (!conn) throw exception::http_exception("No Database Connection Available", 200);
        auto stmt = conn->prepared_statement(
                "select user_id,username,email,real_name,phone_number from t_user where user_id = ? and deleted_at is null and status=1");
        auto test = request.param("__current_user_id");
        stmt->bind_param(0, request.param("__current_user_id"));
        auto result = stmt->execute();
        if (!result || result->count() <= 0) return json_response(nullptr);
        return json_response(result->get_row(0)->to_json());
    }

    std::unique_ptr<obelisk::http_response> user_controller::create(http_request &request) {
        using namespace validator;
        request.validate({
                                 {"username",        {required(), unique("t_user", "username")}},
                                 {"password",        {required(), min_length(8), confirmed()}},
                                 {"real_name",       {required(), min_length(1)}},
                                 {"organization_id", {required(), exists("t_organization", "organization_id")}},
                         });
        auto usr = obelisk::orm::user::instance(request.params());
        usr.password = obelisk::scrypt_hasher::hash(request.param("password"));
        usr.user_id = boost::replace_all_copy(boost::uuids::to_string(boost::uuids::random_generator()()), "-", "");
        usr.create();
        return json_response(usr.to_json());
    }

    std::unique_ptr<obelisk::http_response> user_controller::update(http_request &request) {
        using namespace validator;
        request.validate({
            {"real_name",       {required(), min_length(1)}},
            {"organization_id", {required(), exists("t_organization", "organization_id")}}
        });
        auto usr = obelisk::orm::user::instance(request.params());
        usr.password = obelisk::scrypt_hasher::hash(request.param("password"));
        usr.user_id = boost::replace_all_copy(boost::uuids::to_string(boost::uuids::random_generator()()), "-", "");
        usr.create();
        return json_response(usr.to_json());
    }


}