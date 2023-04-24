//
// Created by Hackman.Lo on 3/20/2023.
//

#ifndef OBELISK_REQUEST_FILE_H
#define OBELISK_REQUEST_FILE_H
#include "string/string.h"

namespace obelisk {
  struct request_file {
      sahara::string original_name;
      sahara::string temp_file_path;
      sahara::string content_type;
  };

} // obelisk

#endif //OBELISK_REQUEST_FILE_H
