//
// Created by Hackman.Lo on 7/21/2023.
//

#include "cryption/bcrypt_hasher.h"
#include "cryption/bcrypt/node_blf.h"
#include "cryption/bcrypt/openbsd.h"
namespace obelisk::cryption {
    std::string bcrypt::generate(const std::string &password, unsigned int rounds) {
        char salt[_SALT_LEN];

        unsigned char seed[17]{};
        arc4random_init();

        arc4random_buf(seed, 16);

        bcrypt_gensalt('b', rounds, seed, salt);

        std::string hash(61, '\0');
        node_bcrypt(password.c_str(), password.size(), salt, &hash[0]);
        hash.resize(60);
        return hash;
    }

    bool bcrypt::validate(const std::string &password, const std::string &hash) {
        std::string got(61, '\0');
        node_bcrypt(password.c_str(), password.size(), hash.c_str(), &got[0]);
        got.resize(60);
        return hash == got;
    }
} // obelisk