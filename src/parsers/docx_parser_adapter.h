#pragma once

#include "legacy_parsers/legacy_docx_parser.h"

class TDocxParserAdapter: public TIDocumentParser {
    std::unique_ptr<TLegacyDocxParser> OldParser;

public:
    TDocxParserAdapter()
        : OldParser(std::make_unique<TLegacyDocxParser>()) {
    }

    explicit TDocxParserAdapter(std::unique_ptr<TLegacyDocxParser> parser)
        : OldParser(std::move(parser)) {
    }

    TDocxParserAdapter(const TDocxParserAdapter&) = delete;
    TDocxParserAdapter& operator=(const TDocxParserAdapter&) = delete;

    TDocxParserAdapter(TDocxParserAdapter&&) = default;
    TDocxParserAdapter& operator=(TDocxParserAdapter&&) = default;
    bool SupportsFormat(const std::string& format) const override {
        return format == "docx";
    }

    TDocumentInfo Parse(const std::string& file_path) override {
        return OldParser->Parse(file_path);
    }
};