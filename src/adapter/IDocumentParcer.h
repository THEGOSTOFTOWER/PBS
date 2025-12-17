#pragma once

#include "dto/DocumentInfo.h"
#include <vector>
#include <filesystem>

class IDocumentParcer {
public:
    virtual ~IDocumentParcer() = default;

    virtual DocumentInfo Parse(const std::string& filePath) = 0;
    virtual bool SupportsFormat(const std::string& format) const = 0;
};