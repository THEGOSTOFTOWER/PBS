#include "legacy_parsers/legacy_text_parser.h"

class TTextParserAdapter: public TIDocumentParser {
    TLegacyTextParser OldParser;

public:
    bool SupportsFormat(const std::string& format) const override {
        return format == "txt";
    }

    TDocumentInfo Parse(const std::string& file_path) override {
        return OldParser.Parse(file_path);
    }
};