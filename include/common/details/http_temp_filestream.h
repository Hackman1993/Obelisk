/***
 * @file   http_temp_filestream
 * @author Hackman.Lo
 * @parent std::fstream
 * @date  2023-07-06
 * @class  http_temp_fstream
 * @description This class is used to create a temporary file stream
 */

#ifndef LIBRARYORGANIZED_HTTP_TEMP_FILESTREAM_H
#define LIBRARYORGANIZED_HTTP_TEMP_FILESTREAM_H
#include <fstream>
namespace obelisk {

    class http_temp_fstream : public std::fstream {
    public:
        explicit http_temp_fstream(std::string path);
        ~http_temp_fstream() override;
        std::string path();

    protected:
        std::string path_;
    };

} // obelisk

#endif //LIBRARYORGANIZED_HTTP_TEMP_FILESTREAM_H
