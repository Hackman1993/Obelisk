/***
 * @file   mime_types.h
 * @description  Header of mime_types.cpp
 * @class  mime_types
 * @date  2023-07-06
 * @description This class contains only static methods, so there is no need to create an instance of this class.
 */

#ifndef LIBRARYORGANIZED_MIME_TYPES_H
#define LIBRARYORGANIZED_MIME_TYPES_H
#include <string>
#include <unordered_map>
namespace obelisk {

    class mime_types {
    public:
        static std::string lookup(const std::string& file_path);
    protected:
        mime_types() = default;
        static std::unordered_map<std::string, std::string> mimes_;
    };

} // obelisk

#endif //LIBRARYORGANIZED_MIME_TYPES_H
