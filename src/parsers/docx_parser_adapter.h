#include "legacy_parsers/legacy_docx_parser.h"

class TDocxParserAdapter: public TIDocumentParser {
    TLegacyDocxParser OldParcer;

public:
    bool SupportsFormat(const std::string& format) const override {
        return format == "docx";
    }

    TDocumentInfo Parse(const std::string& file_path) override {
        return OldParcer.Parse(file_path);
    }
};