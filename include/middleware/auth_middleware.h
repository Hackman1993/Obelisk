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
        static std::unique_ptr<http_response> handle(http_request &request, std::string_view params) {
            if (!request.header().contains("Authorization")) throw exception::http_exception("Access Denied!", 403);

            const sahara::string token_header = "bearer ";
            sahara::string authorization_header = request.header()["Authorization"];
            auto token_itr = boost::ifind_first(authorization_header, token_header);
            if (!token_itr) throw exception::http_exception("Access Denied!", 403);

            sahara::string token = authorization_header.substr(token_itr.size());
            auto connection = DB::get_connection<rosetta::mysql_connection>("default");
            auto stmt = connection->prepared_statement("select obelisk.t_user.user_id from obelisk.t_user, obelisk.t_access_token where t_user.user_id = t_access_token.user_id and t_access_token.access_token_id = ?");

            stmt->bind_param(0, token);
            auto result = stmt->execute();
            if (result->count() <= 0) throw exception::http_exception("Access Denied!", 403);
            request.params().set("__current_user_id", result->get<rosetta::string>(0, "user_id"));
            return nullptr;
        }

        static std::unique_ptr<http_response> permission(http_request &request, std::string_view params) {
            handle(request, params);

            return nullptr;
        }

    };

} // obelisk

#endif //OBELISK_AUTH_MIDDLEWARE_H
