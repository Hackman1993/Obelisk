#include <iostream>

#include "server/http_server.h"
#include <boost/spirit/home/x3.hpp>
#include <boost/url/encode.hpp>

#include "controller/user_controller.h"
#include "database.h"
#include <rosetta/mysql.h>
#include <rosetta/rosetta.h>
#include "middleware/auth_middleware.h"
int main() {
    try {
        g_pool.initialize<rosetta::mysql_connection>();
        obelisk::http_server server("0.0.0.0", 28888);
        server.listen("0.0.0.0", 8082);
        server.route("/api/user/login", &obelisk::user_controller::get_token).add_method("POST").allow_cors("http://localhost:3000");
        server.route("/api/user/permissions", &obelisk::user_controller::get_permission).middleware("", &auth_middleware::handle).add_method("GET").allow_cors("http://localhost:3000");
        server.run();
    } catch (std::exception &e) {
        LOG_MODULE_WARN("obelisk", "{}", e.what());
    }

    return 0;
}