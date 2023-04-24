#include <iostream>
#include "http_server.h"
#include "controller/auth_controller.h"
#include <obelisk.h>
#include <boost/url.hpp>
#include <rosetta.h>
#include "cryption/scrypt_hasher.h"
#include "DB.h"
#include "controller/user_controller.h"


int main() {
    obelisk::DB::make_pool("default", 10, "127.0.0.1", 3306, "root", "hl97005497--",
                           "obelisk").initialize<rosetta::mysql_connection>();

    obelisk::http_server server("0.0.0.0", 28888);
    server.route("/api/user/login", &obelisk::user_controller::get_token).add_method(boost::beast::http::verb::post);

    server.route("/api/user/current", &obelisk::user_controller::current).add_method(boost::beast::http::verb::get)
        .middleware("token_auth", &obelisk::auth_middleware::handle);

    server.route("/api/user/create", &obelisk::user_controller::create).add_method(boost::beast::http::verb::post)
        .middleware("token_auth", &obelisk::auth_middleware::handle);

    server.route("/api/user/{user_id}/update", &obelisk::user_controller::create).add_method(
            boost::beast::http::verb::post).middleware("token_auth", &obelisk::auth_middleware::handle);

    server.route("/api/user/{user_id}/delete", &obelisk::user_controller::create).add_method(
            boost::beast::http::verb::post).middleware("token_auth", &obelisk::auth_middleware::handle);

    server.route("/api/user/{user_id}/password_reset", &obelisk::user_controller::create).add_method(
            boost::beast::http::verb::post).middleware("token_auth", &obelisk::auth_middleware::handle);


    server.listen("0.0.0.0", 8082);
    std::cout << std::thread::hardware_concurrency() << std::endl;
    server.run(std::thread::hardware_concurrency() + 2);

    return 0;
}