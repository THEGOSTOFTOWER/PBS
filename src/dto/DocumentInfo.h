#pragma once

#include "exception/CustomException.h"
#include <map>

struct DocumentInfo {
    std::string text;
    std::map<std::string, std::string> metadata;
    std::size_t page_count = 0;
};