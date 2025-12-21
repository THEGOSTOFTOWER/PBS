#pragma once

#include "document_parser.h"
#include "legacy_parsers/legacy_pdf_parser.h"

#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

class TPdfParserAdapter final: public IDocumentParser {
private:
    std::unique_ptr<TLegacyPdfParser> oldparser_;

public:
    TPdfParserAdapter()
        : oldparser_(std::make_unique<TLegacyPdfParser>()) {
    }

    explicit TPdfParserAdapter(std::unique_ptr<TLegacyPdfParser> parser)
        : oldparser_(std::move(parser)) {
    }

    TPdfParserAdapter(const TPdfParserAdapter&) = delete;
    TPdfParserAdapter& operator=(const TPdfParserAdapter&) = delete;

    TPdfParserAdapter(TPdfParserAdapter&&) = default;
    TPdfParserAdapter& operator=(TPdfParserAdapter&&) = default;

    bool SupportsFormat(const std::string& extension) const override {
        return extension == "pdf";
    }

    TDocumentInfo Parse(const std::string& file_path) override {
        return oldparser_->Parse(file_path);
    }
};