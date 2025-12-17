#include "idocument_parser.h"
#include "legacy_parsers/legacy_pdf_parser.h"
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

class TPdfParserAdapter final: public TIDocumentParser {
private:
    TLegacyPdfParser OldParser;

public:
    bool SupportsFormat(const std::string& extension) const override {
        return extension == "pdf";
    }

    TDocumentInfo Parse(const std::string& file_path) override {
        return OldParser.Parse(file_path);
    }
};