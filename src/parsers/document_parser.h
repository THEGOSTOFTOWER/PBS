#pragma once

#include "dto/document_info.h"

#include <vector>
#include <filesystem>

class IDocumentParser {
public:
    virtual ~IDocumentParser() = default;

    virtual TDocumentInfo Parse(const std::string& filePath) = 0;
    virtual bool SupportsFormat(const std::string& format) const = 0;
};