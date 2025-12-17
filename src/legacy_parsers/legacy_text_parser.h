#include "dto/document_info.h"
#include "fstream"
#include "sstream"

class TLegacyTextParser {
public:
    TDocumentInfo Parse(const std::string& file_path) {
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