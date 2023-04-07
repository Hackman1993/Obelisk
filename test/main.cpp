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
//  obelisk::route_item item("/route/{param1}/{param2}/bacba");
//  std::string_view str = "/route/dga2/param2/bacba";
  obelisk::DB::make_pool("default", 10, "127.0.0.1", 3306, "root","hl97005497--", "obelisk").initialize<rosetta::mysql_connection>();

  obelisk::http_server server("0.0.0.0", 28888);
  //server.add_default_middlewares();
  server.route("/api/user/login", &obelisk::user_controller::get_token).add_method(boost::beast::http::verb::post);
  server.route("/api/user/current", &obelisk::user_controller::current).add_method(boost::beast::http::verb::get).middleware("token_auth", &obelisk::auth_middleware::handle);
  server.route("/api/user/create", &obelisk::user_controller::create).add_method(boost::beast::http::verb::post).middleware("token_auth", &obelisk::auth_middleware::handle);



  server.listen("0.0.0.0", 8082);
  std::cout << std::thread::hardware_concurrency() << std::endl;
  server.run(std::thread::hardware_concurrency() + 2);

  return 0;
}




//
//int main(int argc, char* argv[])
//{
//  auto const address = boost::asio::ip::make_address("0.0.0.0");
//  auto const port = static_cast<unsigned short>(82);
//  auto const doc_root = std::make_shared<std::string>("./wwwroot");
//  auto const threads = std::max<int>(1, 10);
//
//  obelisk::http_server server("0.0.0.0", 8082);
//  server.listen("0.0.0.0", 8083);
//  server.run(1);
//
//  return 0;
//}