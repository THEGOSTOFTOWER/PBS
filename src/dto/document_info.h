#pragma once

#include "exception/custom_exceptions.h"

#include <map>

struct TDocumentInfo {
    std::string text;
    std::map<std::string, std::string> metadata;
};