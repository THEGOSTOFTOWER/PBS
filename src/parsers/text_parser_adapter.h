#include "legacy_parsers/legacy_text_parser.h"

class TTextParserAdapter: public TIDocumentParser {
    std::unique_ptr<TLegacyTextParser> OldParser;

public:
    TTextParserAdapter()
        : OldParser(std::make_unique<TLegacyTextParser>()) {
    }

    explicit TTextParserAdapter(std::unique_ptr<TLegacyTextParser> parser)
        : OldParser(std::move(parser)) {
    }

    TTextParserAdapter(const TTextParserAdapter&) = delete;
    TTextParserAdapter& operator=(const TTextParserAdapter&) = delete;

    TTextParserAdapter(TTextParserAdapter&&) = default;
    TTextParserAdapter& operator=(TTextParserAdapter&&) = default;
    bool SupportsFormat(const std::string& format) const override {
        return format == "txt";
    }

    TDocumentInfo Parse(const std::string& file_path) override {
        return OldParser->Parse(file_path);
    }
};