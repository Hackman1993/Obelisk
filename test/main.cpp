#include <iostream>

#include "server/http_server.h"
#include <boost/spirit/home/x3.hpp>
#include <boost/url/encode.hpp>
unsigned char ToHex(unsigned char x)
{
  return  x > 9 ? x + 55 : x + 48;
}

std::string UrlEncode(const std::string& str)
{
  std::ostringstream ostream;
  size_t length = str.length();
  for (size_t i = 0; i < length; i++)
  {
    if (isalnum((unsigned char)str[i]) || (str[i] == '-') || (str[i] == '_') || (str[i] == '.') || (str[i] == '~')) ostream << str[i];
    else ostream << '%' << ToHex((unsigned char)str[i] >> 4) << ToHex((unsigned char)str[i] % 16);
  }
  return ostream.str();
}
unsigned char FromHex(unsigned char x)
{
  unsigned char y;
  if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
  else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
  else if (x >= '0' && x <= '9') y = x - '0';
  else assert(0);
  return y;
}
std::string UrlDecode(const std::string& str)
{
  std::ostringstream ostream;
  size_t length = str.length();
  for (size_t i = 0; i < length; i++)
  {
    if (str[i] == '%')
    {
      assert(i + 2 < length);
      unsigned char high = FromHex((unsigned char)str[++i]);
      unsigned char low = FromHex((unsigned char)str[++i]);
      ostream<< char(high * 16 + low);
    }
    else ostream << str[i];
  }
  return ostream.str();
}
int main() {
//    obelisk::DB::make_pool("default", 10, "127.0.0.1", 3306, "root", "hl97005497--",
//                           "obelisk").initialize<rosetta::mysql_connection>();
//
    sahara::string s = "罗浩然 ";
    s.url_encode();
    s.url_decode();
    //std::cout << boost::url::encode(opt.data(),opt.size(),s,);
    obelisk::http_server server("0.0.0.0", 28888);
    server.listen("0.0.0.0", 8082);
    server.run(std::thread::hardware_concurrency() + 2);
//    server.route("/api/user/login", &obelisk::user_controller::get_token).add_method(boost::beast::http::verb::post);
//
//    server.route("/api/user/current", &obelisk::user_controller::current).add_method(boost::beast::http::verb::get)
//        .middleware("token_auth", &obelisk::auth_middleware::handle);
//
//    server.route("/api/user/create", &obelisk::user_controller::create).add_method(boost::beast::http::verb::post)
//        .middleware("token_auth", &obelisk::auth_middleware::handle);
//
//    server.route("/api/user/{user_id}/update", &obelisk::user_controller::create).add_method(
//            boost::beast::http::verb::post).middleware("token_auth", &obelisk::auth_middleware::handle);
//
//    server.route("/api/user/{user_id}/delete", &obelisk::user_controller::create).add_method(
//            boost::beast::http::verb::post).middleware("token_auth", &obelisk::auth_middleware::handle);
//
//    server.route("/api/user/{user_id}/password_reset", &obelisk::user_controller::create).add_method(
//            boost::beast::http::verb::post).middleware("token_auth", &obelisk::auth_middleware::handle);
//
//
//    server.listen("0.0.0.0", 8082);
//    std::cout << std::thread::hardware_concurrency() << std::endl;
//    server.run(std::thread::hardware_concurrency() + 2);

    return 0;
}