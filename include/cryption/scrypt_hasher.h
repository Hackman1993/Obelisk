//
// Created by Hackman.Lo on 3/28/2023.
//

#ifndef OBELISK_SCRYPT_HASHER_H
#define OBELISK_SCRYPT_HASHER_H

#include <cryptopp/scrypt.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>

namespace obelisk {

    class scrypt_hasher {

    public:
        static sahara::string random_salt() {
            std::string salt;
            salt.resize(8);
            CryptoPP::AutoSeededRandomPool prng;
            prng.GenerateBlock(reinterpret_cast<CryptoPP::byte *>(salt.data()), salt.size());

            std::string salt_hex;
            CryptoPP::StringSource(salt, true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(salt_hex)));
            return salt_hex;
        }

        static sahara::string hash(std::string_view data, std::string salt = random_salt()) {

            CryptoPP::SecByteBlock derived(64);
            CryptoPP::Scrypt scrypt;
            scrypt.DeriveKey(derived, derived.size(), reinterpret_cast<const CryptoPP::byte *>(data.data()),
                             data.size(),
                             reinterpret_cast<const CryptoPP::byte *>(salt.data()), salt.size());
            std::string result;
            CryptoPP::StringSource(derived, derived.size(), true,
                                   new CryptoPP::HexEncoder(new CryptoPP::StringSink(result)));
            result = salt + "$" + result;
            return result;
        }

        static bool check(const sahara::string& hashed, const sahara::string& data) {
            std::vector<std::string> data_split;
            //TODO: Implement
            //boost::split(data_split, hashed, boost::is_any_of("$"));
            //sahara::string rehash = hash(data, data_split[0]);
            //return hashed == rehash;
            return true;
        }

    };

} // obelisk

#endif //OBELISK_SCRYPT_HASHER_H
