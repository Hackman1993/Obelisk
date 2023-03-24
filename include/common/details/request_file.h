//
// Created by Hackman.Lo on 3/20/2023.
//

#ifndef OBELISK_REQUEST_FILE_H
#define OBELISK_REQUEST_FILE_H
#include <fstream>
#include <filesystem>
namespace obelisk {
  struct request_file {
    std::string original_name;
    std::string temp_file_path;
    std::string content_type;
  };

} // obelisk

#endif //OBELISK_REQUEST_FILE_H
