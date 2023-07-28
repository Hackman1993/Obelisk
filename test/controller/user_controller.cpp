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
#include <cryption/bcrypt_hasher.h>
#include <rosetta/rosetta.h>
#include <rosetta/mysql.h>

namespace obelisk {
    std::shared_ptr<obelisk::http_response> user_controller::get_token(http_request &request) {
        request.validate({{"login",    {validator::required()}},
                          {"password", {validator::required(), validator::min_length(6)}}});
        std::string username = std::get<std::string>(request.param("login"));
        std::string password = std::get<std::string>(request.param("password"));

        rosetta::core::select select_builder({"user_id", "username", "password", "token"});
        auto te = select_builder.from({{"t_user", ""}}).where("username", "=", "?").where("deleted_at", "is", "null").left_join(rosetta::core::alia{"t_user_access_token", "b"},[&](auto &join){
            join.on("user_id", "=", "b.fn_user_id");
        });
        auto connection = g_pool.get_connection<rosetta::mysql_connection>();
        if(!connection) throw exception::validation_exception("Failed to get connection!");
        auto s = select_builder.compile();

        auto statement = connection->prepared_statement(select_builder.compile());
        if(!statement) throw exception::validation_exception("Create Statement Failed");
        statement->bind_param(username);
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
            if(!statement) throw exception::validation_exception("Create Statement Failed");
            statement->bind_param(uid);
            statement->bind_param(token);
            statement->execute();
        }else{
            token = std::get<std::string>(vtoken);
        }

        return json_response(boost::json::object({{"user",        {{"user_id", uid}, {"username", uname}}},
                                                  {"token",       token}}));

    }

    struct Visitor {
        Visitor(boost::json::object& obj, const std::string& str): json_(obj), name_(str){}
        void operator()(std::int64_t value) {
            json_[name_] = value;
        }

        void operator()(bool value) {
            json_[name_] = value;
        }

        void operator()(double value) {
            json_[name_] = double(value);
        }

        void operator()(std::string value) {
            json_[name_] = value;
        }

        void operator()(std::uint64_t value) {
            json_[name_] = value;
        }

        void operator()(sahara::time::timestamp value) {
            json_[name_] = nullptr;
        }

        void operator()(std::nullptr_t value) {
            json_[name_] = value;
        }
    protected:
        const std::string& name_;
        boost::json::object& json_;
    };

    typedef std::variant<std::nullptr_t, int> var_t;

    template<typename ... Types>struct Overloads : Types ... {using Types::operator()...;};
    template<typename ... Types> Overloads(Types...) -> Overloads<Types...>;
    std::shared_ptr<obelisk::http_response> user_controller::get_permission(http_request &request) {
        boost::json::array permissions;
        boost::json::array menus;
        boost::json::object user;
        std::unordered_map<std::uint64_t, std::shared_ptr<boost::json::object>> menu_map;
        auto connection = g_pool.get_connection<rosetta::mysql_connection>();
        {
            rosetta::core::select user_builder({"user_id", "username", "email", "real_name", "phone_number", "passport_no", "photo_url", "avatar_url"});
            user_builder.from({{"t_user", ""}}).where("user_id", "=", "?");
            if(!connection) throw exception::validation_exception("Failed to get connection!");
            auto statement = connection->prepared_statement(user_builder.compile());
            statement->bind_param(request.param("__current_user_id"));
            statement->execute();
            auto result = statement->get();
            if(!result) throw exception::validation_exception("Failed to get connection!");
            while(auto row = result->next()){
                std::visit(Visitor(user, "user_id"), row->get_column(0));
                std::visit(Visitor(user, "username"), row->get_column(1));
                std::visit(Visitor(user, "email"), row->get_column(2));
                std::visit(Visitor(user, "real_name"), row->get_column(3));
                std::visit(Visitor(user, "phone_number"), row->get_column(4));
                std::visit(Visitor(user, "passport_no"), row->get_column(5));
                std::visit(Visitor(user, "photo_url"), row->get_column(6));
                std::visit(Visitor(user, "avatar_url"), row->get_column(7));
            }
        }
        {
            rosetta::core::select permission_builder({"code"});
            permission_builder.from({{"t_permission", "a"}}).inner_join({"t_mid_role_permission", "b"}, [&](auto &join){
                join.on("a.permission_id", "=", "b.fn_permission_id");
            }).inner_join({"t_mid_user_role", "c"}, [&](auto &join){
                join.on("b.fn_role_id", "=", "c.fn_role_id");
            }).inner_join({"t_user_access_token", "d"}, [&](auto &join){
                join.on("c.fn_user_id", "=", "d.fn_user_id");
            }).where("d.token", "=", "?");
            if(!connection) throw exception::validation_exception("Failed to get connection!");
            auto statement = connection->prepared_statement(permission_builder.compile());
            statement->bind_param(std::get<std::string>(request.param("__current_token")));
            statement->execute();
            auto result = statement->get();
            if(!result) throw exception::validation_exception("Failed to get connection!");
            while(auto row = result->next()){
                permissions.push_back(boost::json::value(std::get<std::string>(row->get_column(0))));
            }
        }
        {
            rosetta::core::select menu_builder({"menu_id", "menu_code", "menu_text", "description", "url", "additional_config", "fn_menu_id"});
            menu_builder.from({{"t_menu", "a"}}).left_join({"t_mid_role_permission", "b"}, [&](auto &join){
                join.on("a.fn_permission_id", "=", "b.fn_permission_id");
            }).left_join({"t_mid_user_role", "c"}, [&](auto &join){
                join.on("b.fn_role_id", "=", "c.fn_role_id");
            }).where("fn_user_id", "is", "null").or_("c.fn_user_id", "=", "?").order_asc({"fn_menu_id"});
            auto sql = menu_builder.compile();
            if(!connection) throw exception::validation_exception("Failed to get connection!");
            auto statement = connection->prepared_statement(menu_builder.compile());
            statement->bind_param(request.param("__current_user_id"));
            statement->execute();
            auto result = statement->get();
            if(!result) throw exception::validation_exception("Failed to get connection!");


            while(auto row = result->next()){
                std::shared_ptr<boost::json::object> obj = std::make_shared<boost::json::object>();
                std::visit(Visitor(*obj, "menu_id"), row->get_column(0));
                std::visit(Visitor(*obj, "menu_code"), row->get_column(1));
                std::visit(Visitor(*obj, "menu_text"), row->get_column(2));
                std::visit(Visitor(*obj, "description"), row->get_column(3));
                std::visit(Visitor(*obj, "url"), row->get_column(4));
                std::visit(Visitor(*obj, "additional_config"), row->get_column(5));
                std::visit(Visitor(*obj, "parent_id"), row->get_column(6));

                if((*obj)["parent_id"].is_null()){
                    auto menu_id = (*obj)["menu_id"].as_uint64();
                    auto pair = menu_map.emplace(menu_id, obj);
                    menus.emplace_back(*menu_map.at(menu_id));
                    continue;
                }
                std::uint64_t parent_id = (*obj)["parent_id"].as_uint64();

                if(menu_map.contains(parent_id)){
                    auto &ref = menu_map.at(parent_id);
                    if(!(*ref).contains("children")){
                        (*ref)["children"] = boost::json::array();
                    }
                    auto pair = menu_map.emplace((*ref)["menu_id"].as_uint64(), obj);
                    (*ref)["children"].as_array().emplace_back(pair.second);
                }else{}
            }
            user["menus"] = menus;
        }

        user["permissions"] = permissions;
        return json_response(user);
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