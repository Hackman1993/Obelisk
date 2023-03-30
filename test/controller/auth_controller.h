//
// Created by Hackman.Lo on 3/21/2023.
//

#ifndef OBELISK_AUTH_CONTROLLER_H
#define OBELISK_AUTH_CONTROLLER_H

#include "controller/controller_base.h"
#include "common/response/string_response.h"
#include <iostream>
#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
class auth_controller{
public:
  static std::unique_ptr<obelisk::http_response> test(obelisk::http_request &request){

    std::string uuid = boost::uuids::to_string(boost::uuids::random_generator()());
    std::string iv="\x01\x02\x03\x04\x05\x06\x07\x08\x09\x10\x11\x12\x13\x14\x15\x16";
    std::string key="\x01\x02\x03\x04\x05\x06\x07\x08\x09\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x30\x31\x32";

    CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryption(reinterpret_cast<const CryptoPP::byte *>(key.c_str()), key.length(), reinterpret_cast<const CryptoPP::byte *>(iv.c_str()));

    for(int i=0;i<5;i++) {
      std::string encrypted;
      CryptoPP::StringSource encryptor(uuid, true, new CryptoPP::StreamTransformationFilter(encryption, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encrypted), false)));
      uuid = encrypted;
    }
    return std::make_unique<obelisk::string_response>(200, "{\"code\":0 , \"content\":\"" + uuid + "\" , \"msg\":\"SUCCESS\"}");
  }
};


#endif //OBELISK_AUTH_CONTROLLER_H
