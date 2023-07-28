//
// Created by Hackman.Lo on 7/24/2023.
//

#include "auth_middleware.h"
#include <rosetta/mysql.h>

std::shared_ptr<obelisk::http_response> auth_middleware::handle(obelisk::http_request &request, const sahara::string &params){
    if (!request.header().contains("Authorization")) throw obelisk::exception::http_exception("Access Denied!", 403);

    sahara::string authorization_header = request.header()["Authorization"];
    auto pos = authorization_header.ifind_first("Bearer ");
    if(pos == -1)
        throw obelisk::exception::http_exception("Access Denied!", 403);
    std::string token = authorization_header.substr(pos).to_std();
    auto permissions = params.split(",");

    rosetta::core::select builder({"code", "c.fn_user_id", "token"});
    builder.from({{"t_permission", "a"}}).inner_join({"t_mid_role_permission", "b"},[&](auto& join){
        join.on("a.permission_id", "=", "b.fn_permission_id");
    }).inner_join({"t_mid_user_role", "c"}, [&](auto& join){
        join.on("b.fn_role_id", "=", "c.fn_role_id");
    }).inner_join({"t_user_access_token", "d"}, [&](auto& join){
        join.on("c.fn_user_id", "=", "d.fn_user_id");
    }).where("token", "=", "?");
    if(!permissions.empty()){
        builder.where_in("code", permissions.size());
    }

    auto test = builder.compile();
    auto connection = g_pool.get_connection<rosetta::mysql_connection>();
    std::shared_ptr<rosetta::mysql_statement> statement = connection->prepared_statement(builder.compile());
    if(!statement) return nullptr;
    statement->bind_param(token);
    if(!permissions.empty()){
        for(auto &permission: permissions){
            statement->bind_param(permission.to_std());
        }
    }
    statement->execute();
    auto result = statement->get();
    if(!permissions.empty() && permissions.size() != result->count()){
        throw obelisk::exception::http_exception("Access Denied!", 403);
    }
    if (auto row = result->next()) {
        request.params().set("__current_user_id", std::get<std::uint64_t>(row->get_column(1)));
        request.params().set("__current_token", token);
        return nullptr;
    } else
        throw obelisk::exception::http_exception("Access Denied!", 403);
    return nullptr;
}