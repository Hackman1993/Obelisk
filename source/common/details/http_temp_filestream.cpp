//
// Created by Hackman.Lo on 7/6/2023.
//
#include <filesystem>
#include "common/details/http_temp_filestream.h"

namespace obelisk {
    http_temp_fstream::http_temp_fstream(std::string path) :path_(path){
        std::ofstream fstream(path_, std::ios::out | std::ios::binary);
        fstream.close();
        this->open(path_, std::ios::out | std::ios::in | std::ios::binary);

    }

    http_temp_fstream::~http_temp_fstream() {
        this->close();
        if(std::filesystem::exists(path_) && std::filesystem::is_regular_file(path_))
            std::filesystem::remove(path_);
    }

    std::string http_temp_fstream::path() {
        return path_;
    }
} // obelisk