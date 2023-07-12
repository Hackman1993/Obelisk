//
// Created by Hackman.Lo on 3/20/2023.
//

#ifndef OBELISK_REQUEST_FILE_H
#define OBELISK_REQUEST_FILE_H
#include <sahara/string/string.h>
#include <filesystem>
namespace obelisk {
  struct request_file {
      sahara::string original_name_;
      sahara::string temp_file_path_;
      sahara::string content_type_;
      ~request_file(){
        if(std::filesystem::exists(std::filesystem::path(temp_file_path_.to_std())))
          std::filesystem::remove(temp_file_path_.to_std());
      }
  };

} // obelisk

#endif //OBELISK_REQUEST_FILE_H
