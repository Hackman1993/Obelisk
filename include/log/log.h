//
// Created by Hackman.Lo on 7/5/2023.
//

#ifndef LIBRARYORGANIZED_LOG_H
#define LIBRARYORGANIZED_LOG_H
#include <boost/log/trivial.hpp>
namespace obelisk {

    class log {
    public:
        static void info(const std::string& msg){
            std::cout << msg << std::endl;
        }
    };

} // obelisk

#endif //LIBRARYORGANIZED_LOG_H
