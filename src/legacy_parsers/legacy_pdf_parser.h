#pragma once

#include "dto/document_info.h"

#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

class TLegacyPdfParser {
public:
    virtual TDocumentInfo Parse(const std::string& file_path) {
        try {
            std::unique_ptr<poppler::document> doc(poppler::document::load_from_file(file_path));
            if (!doc) {
                throw TDocumentProcessingException("Failed to load PDF document");
            }
            std::string text;
            const int num_pages = doc->pages();
            for (int i = 0; i < num_pages; ++i) {
                std::unique_ptr<poppler::page> p(doc->create_page(i));
                if (p) {
                    poppler::byte_array ba = p->text().to_utf8();
                    text.append(ba.data(), ba.size());
                }
            }
            return {std::move(text), {{"format", "pdf"}}};
        } catch (const std::exception& e) {
            throw TParsingException(file_path, std::string(e.what()));
        }
    }
};