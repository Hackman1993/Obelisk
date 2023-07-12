//
// Created by Hackman.Lo on 7/4/2023.
//

#include "common/details/mime_types.h"
#include <filesystem>
#include <exception/http_exception.h>
namespace obelisk {
    std::unordered_map<std::string, std::string> mime_types::mimes_ = std::unordered_map<std::string, std::string>({
       { ".aac", "audio/aac"},
       { ".abw", "application/x-abiword"},
       { ".arc", "application/x-freearc"},
       { ".avif", "image/avif"},
       { ".avi", "video/x-msvideo"},
       { ".azw", "application/vnd.amazon.ebook"},
       { ".bin", "application/octet-stream"},
       { ".bmp", "image/bmp"},
       { ".bz", "application/x-bzip"},
       { ".bz2", "application/x-bzip2"},
       { ".cda", "application/x-cdf"},
       { ".csh", "application/x-csh"},
       { ".css", "text/css"},
       { ".csv", "text/csv"},
       { ".doc", "application/msword"},
       { ".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
       { ".eot", "application/vnd.ms-fontobject"},
       { ".epub", "application/epub+zip"},
       { ".gz", "application/gzip"},
       { ".gif", "image/gif"},
       { ".html", "text/html"},
       { ".htm", "text/html"},
       { ".ico", "image/vnd.microsoft.icon"},
       { ".ics", "text/calendar"},
       { ".jar", "application/java-archive"},
       { ".jpg", "image/jpeg"},
       { ".jpeg", "image/jpeg"},
       { ".js", "text/javascript"},
       { ".json", "application/json"},
       { ".jsonld", "application/ld+json"},
       { ".midi", "audio/x-midi"},
       { ".mid", "audio/midi"},
       { ".mjs", "text/javascript"},
       { ".mp3", "audio/mpeg"},
       { ".mp4", "video/mp4"},
       { ".mpeg", "video/mpeg"},
       { ".mpkg", "application/vnd.apple.installer+xml"},
       { ".odp", "application/vnd.oasis.opendocument.presentation"},
       { ".ods", "application/vnd.oasis.opendocument.spreadsheet"},
       { ".odt", "application/vnd.oasis.opendocument.text"},
       { ".oga", "audio/ogg"},
       { ".ogv", "video/ogg"},
       { ".ogx", "application/ogg"},
       { ".opus", "audio/opus"},
       { ".otf", "font/otf"},
       { ".png", "image/png"},
       { ".pdf", "application/pdf"},
       { ".php", "application/x-httpd-php"},
       { ".ppt", "application/vnd.ms-powerpoint"},
       { ".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
       { ".rar", "application/vnd.rar"},
       { ".rtf", "application/rtf"},
       { ".sh", "application/x-sh"},
       { ".svg", "image/svg+xml"},
       { ".tar", "application/x-tar"},
       { ".tiff", "image/tiff"},
       { ".tif", "image/tiff"},
       { ".ts", "video/mp2t"},
       { ".ttf", "font/ttf"},
       { ".txt", "text/plain"},
       { ".vsd", "application/vnd.visio"},
       { ".wav", "audio/wav"},
       { ".weba", "audio/webm"},
       { ".webm", "video/webm"},
       { ".webp", "image/webp"},
       { ".woff", "font/woff"},
       { ".woff2", "font/woff2"},
       { ".xhtml", "application/xhtml+xml"},
       { ".xls", "application/vnd.ms-excel"},
       { ".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
       { ".xml", "application/xml"},
       { ".xul", "application/vnd.mozilla.xul+xml"},
       { ".zip", "application/zip"},
       { ".3gp", "video/3gpp"},
       { ".3g2", "video/3gpp2"},
       { ".7z", "application/x-7z-compressed"},
    });

    std::string mime_types::lookup(const std::string& file_path) {
        if(!std::filesystem::exists(file_path) || std::filesystem::is_directory(file_path)) {
            throw obelisk::exception::http_exception(404, "File not found");
        }
        std::filesystem::path path(file_path);
        auto test = path.extension().string();
        if(!mimes_.contains(path.extension().string())){
            return "application/octet-stream";
        }
        return mimes_[path.extension().string()];
    }
} // obelisk