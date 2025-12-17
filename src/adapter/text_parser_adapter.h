#pragma once

#include "idocument_parser.h"
#include "fstream"
#include "sstream"

class TTextParserAdapter: public TIDocumentParser {
public:
    bool SupportsFormat(const std::string& format) const override {
        return format == "txt";
    }

    TDocumentInfo Parse(const std::string& file_path) override {
        try {
            std::ifstream file(file_path);
            if (!file.is_open()) {
                throw TUnsupportedFormatException("Failed to open TXT file");
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            return {buffer.str(), {{"format", "txt"}}};
        } catch (const std::exception& e) {
            throw TParsingException(file_path, e.what());
        }
    }
};