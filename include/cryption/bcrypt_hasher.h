//
// Created by Hackman.Lo on 7/21/2023.
//

#ifndef OBELISK_BCRYPT_HASHER_H
#define OBELISK_BCRYPT_HASHER_H
#include <string>
namespace obelisk::cryption {

    class bcrypt {
    public:
        static std::string generate(const std::string& password, unsigned rounds = 10);
        static bool validate(const std::string& password, const std::string& hash);
    };

} // obelisk

#endif //OBELISK_BCRYPT_HASHER_H
