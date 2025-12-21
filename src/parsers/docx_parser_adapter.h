#pragma once

#include "legacy_parsers/legacy_docx_parser.h"

class TDocxParserAdapter: public IDocumentParser {
    std::unique_ptr<TLegacyDocxParser> oldparser_;

public:
    TDocxParserAdapter()
        : oldparser_(std::make_unique<TLegacyDocxParser>()) {
    }

    explicit TDocxParserAdapter(std::unique_ptr<TLegacyDocxParser> parser)
        : oldparser_(std::move(parser)) {
    }

    TDocxParserAdapter(const TDocxParserAdapter&) = delete;
    TDocxParserAdapter& operator=(const TDocxParserAdapter&) = delete;

    TDocxParserAdapter(TDocxParserAdapter&&) = default;
    TDocxParserAdapter& operator=(TDocxParserAdapter&&) = default;
    bool SupportsFormat(const std::string& format) const override {
        return format == "docx";
    }

    TDocumentInfo Parse(const std::string& file_path) override {
        return oldparser_->Parse(file_path);
    }
};