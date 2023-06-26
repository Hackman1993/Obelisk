//
// Created by Hackman.Lo on 6/25/2023.
//

#ifndef LIBRARYORGANIZED_HTTP_TEMP_STREAM_H
#define LIBRARYORGANIZED_HTTP_TEMP_STREAM_H

#include <fstream>
#include <sstream>
#include <iostream>

namespace obelisk {
  class http_temp_stream : public std::iostream {
  public:
    http_temp_stream(std::stringstream &stream) : std::iostream(std::move(stream)) {
      is_file_ = false;
    }

    http_temp_stream(std::string_view file_path) : std::iostream(std::move(std::fstream(file_path.data(), std::ios::in | std::ios::out | std::ios::binary))), path_(file_path) {
      is_file_ = true;
    }

    bool is_file(){
      return is_file_;
    }

    std::string path(){
      return path_;
    }

  protected:
    bool is_file_ = false;
    std::string path_;

  };
}

#endif //LIBRARYORGANIZED_HTTP_TEMP_STREAM_H
