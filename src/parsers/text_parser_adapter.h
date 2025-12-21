#pragma once

#include "legacy_parsers/legacy_text_parser.h"

class TTextParserAdapter: public IDocumentParser {
    std::unique_ptr<TLegacyTextParser> oldparser_;

public:
    TTextParserAdapter()
        : oldparser_(std::make_unique<TLegacyTextParser>()) {
    }

    explicit TTextParserAdapter(std::unique_ptr<TLegacyTextParser> parser)
        : oldparser_(std::move(parser)) {
    }

    TTextParserAdapter(const TTextParserAdapter&) = delete;
    TTextParserAdapter& operator=(const TTextParserAdapter&) = delete;

    TTextParserAdapter(TTextParserAdapter&&) = default;
    TTextParserAdapter& operator=(TTextParserAdapter&&) = default;
    bool SupportsFormat(const std::string& format) const override {
        return format == "txt";
    }

    TDocumentInfo Parse(const std::string& file_path) override {
        return oldparser_->Parse(file_path);
    }
};