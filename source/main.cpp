

#include <http_server.h>

int main(int argc, char* argv[])
{
  auto const address = boost::asio::ip::make_address("0.0.0.0");
  auto const port = static_cast<unsigned short>(82);
  auto const doc_root = std::make_shared<std::string>("./wwwroot");
  auto const threads = std::max<int>(1, 10);

  boost::asio::io_context ioc{threads};

  boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};
  load_server_certificate(ctx);
  std::make_shared<listener>( ioc, ctx, tcp::endpoint{address, port}, doc_root)->run();

  boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
  signals.async_wait( [&](beast::error_code const&, int) { ioc.stop(); });

  std::vector<std::thread> v;
  v.reserve(threads - 1);

  for(auto i = threads - 1; i > 0; --i)
    v.emplace_back( [&ioc] { ioc.run(); });
  ioc.run();
  for(auto& t : v)
    t.join();

  return 0;
}