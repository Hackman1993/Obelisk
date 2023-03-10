#include "virtual_host.h"
#include "exception/system_exception.h"
#include <fstream>
#include <filesystem>

namespace obelisk{
  void virtual_host::load_certificate(const std::string certificate_path, const std::string private_key_path) {
    std::ifstream pk_fstream;
    std::ifstream certificate_fstream;
    try {
      pk_fstream.open(private_key_path);
      certificate_fstream.open(certificate_path);
      unsigned long crt_size = std::filesystem::file_size(certificate_path);
      unsigned long pk_size = std::filesystem::file_size(private_key_path);
      if(crt_size > 4096 || pk_size > 4096)
        throw exception::system_exception("Invalid Certificate!");
      std::string crt_data, pk_data;
      pk_data.reserve(pk_size);
      crt_data.reserve(crt_size);
      certificate_fstream.read(crt_data.data(), crt_size);
      pk_fstream.read(pk_data.data(), pk_size);
      ctx_.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv3);
      ctx_.use_certificate_chain(boost::asio::buffer(crt_data.data(), crt_size));
      ctx_.use_private_key(boost::asio::buffer(pk_data.data(), pk_size), boost::asio::ssl::context::file_format::pem);
    } catch (std::exception &e) {
      pk_fstream.close();
      certificate_fstream.close();
      throw exception::system_exception{e.what()};
    }
    pk_fstream.close();
    certificate_fstream.close();
  }
}
