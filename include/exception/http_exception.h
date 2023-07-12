//
// Created by Hackman.Lo on 6/29/2023.
//

#ifndef LIBRARYORGANIZED_HTTP_EXCEPTION_H
#define LIBRARYORGANIZED_HTTP_EXCEPTION_H

#include "exception_base.h"
namespace obelisk::exception {

    class http_exception: public exception_base {
    public:
        http_exception(unsigned int respcode, const std::string &message) : exception_base(message), code_(respcode) {}

        std::uint32_t code(){
            return code_;
        };

    protected:
        std::uint32_t code_;
    };

} // sahara

#endif //LIBRARYORGANIZED_HTTP_EXCEPTION_H
