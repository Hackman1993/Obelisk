#include <iostream>
#include <http_server.h>
#include "route/http_router.h"

class Controller{
public:
  static std::unique_ptr<obelisk::http_response> test(obelisk::http_request &request){
    for(auto & rou: request.route_params()){
      std::cout << rou.first <<":" << rou.second << std::endl;
    }
    return std::make_unique<obelisk::string_response>(200, "HDKFHA");
  }
};
int main() {
//  obelisk::route_item item("/route/{param1}/{param2}/bacba");
//  std::string_view str = "/route/dga2/param2/bacba";

  obelisk::http_server server("0.0.0.0", 8082);
  server.route("/api/{name?}", std::bind(&Controller::test, std::placeholders::_1)).add_method(boost::beast::http::verb::get);


  server.listen("0.0.0.0", 8083);
  server.run(1);

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