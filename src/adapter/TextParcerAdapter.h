#pragma once

#include "IDocumentParcer.h"
#include "fstream"
#include "sstream"

class TextParcerAdapter: public IDocumentParcer {
public:
    bool SupportsFormat(const std::string& format) const override {
        return format == "txt";
    }

    DocumentInfo Parse(const std::string& file_path) override {
        try {
            std::ifstream file(file_path);
            if (!file.is_open()) {
                throw UnsupportedFormatException("Failed to open TXT file");
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            return {buffer.str(), {{"format", "txt"}}};
        } catch (const std::exception& e) {
            throw ParsingException(file_path, e.what());
        }
    }
};