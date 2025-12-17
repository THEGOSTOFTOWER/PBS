#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cctype>
#include "idocument_parser.h"
#include "pdf_parser_adapter.h"
#include "docx_parser_adapter.h"
#include "text_parser_adapter.h"

class TParserFactory {
private:
    using ParserCreator = std::function<std::unique_ptr<TIDocumentParser>()>;
    std::unordered_map<std::string, ParserCreator> registeredParsers_;

    TParserFactory();

public:
    static TParserFactory& GetInstance();

    void RegisterParser(const std::string& format, ParserCreator creator);

    std::unique_ptr<TIDocumentParser> CreateParser(const std::string& format) const;

    std::unique_ptr<TIDocumentParser> CreateParserForFile(const std::string& filePath) const;

    std::vector<std::string> GetSupportedFormats() const;
};