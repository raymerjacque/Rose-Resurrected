#include "Compat.h"

#ifndef _WIN32
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <sstream>

static bool case_insensitive_resolve(const std::string& path, std::string& resolved) {
    if (path.empty()) return false;

    std::string current;
    size_t start = 0;
    if (path[0] == '/') {
        current = "/";
        start = 1;
    } else {
        current = ".";
    }

    std::stringstream ss(path.substr(start));
    std::string segment;
    std::vector<std::string> segments;
    while (std::getline(ss, segment, '/')) {
        if (!segment.empty() && segment != ".") {
            segments.push_back(segment);
        }
    }

    for (size_t i = 0; i < segments.size(); ++i) {
        const std::string& seg = segments[i];
        if (seg == "..") {
            current += "/..";
            continue;
        }

        DIR* dir = opendir(current.c_str());
        if (!dir) return false;

        struct dirent* entry;
        bool found = false;
        std::string matched_name;
        while ((entry = readdir(dir)) != NULL) {
            if (strcasecmp(entry->d_name, seg.c_str()) == 0) {
                matched_name = entry->d_name;
                found = true;
                break;
            }
        }
        closedir(dir);

        if (!found) return false;

        if (current == "." || current == "/") {
            if (current == "/") current = "/" + matched_name;
            else current = matched_name;
        } else {
            current += "/" + matched_name;
        }
    }

    resolved = current;
    return true;
}

FILE* rose_fopen(const char* filename, const char* mode) {
    if (!filename || !mode) return NULL;

    // Normalize Windows backslashes to forward slashes
    std::string s(filename);
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] == '\\') s[i] = '/';
    }

    // Undefine the macro so we call the real standard libc fopen
    #undef fopen
    FILE* fp = ::fopen(s.c_str(), mode);
    if (fp) return fp;

    // If read mode and file wasn't found, attempt case-insensitive resolution
    if (strchr(mode, 'r') != NULL) {
        std::string resolved;
        if (case_insensitive_resolve(s, resolved)) {
            fp = ::fopen(resolved.c_str(), mode);
            if (fp) return fp;
        }
    }

    return NULL;
}

#endif
