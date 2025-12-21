#pragma once

#include "document_parser.h"

#include <memory>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cctype>

class TParserFactory {
private:
    using ParserCreator = std::function<std::unique_ptr<IDocumentParser>()>;
    std::unordered_map<std::string, ParserCreator> registeredParsers_;

    TParserFactory();

public:
    static TParserFactory& GetInstance();

    void RegisterParser(const std::string& format, ParserCreator creator);

    std::unique_ptr<IDocumentParser> CreateParser(const std::string& format) const;

    std::unique_ptr<IDocumentParser> CreateParserForFile(const std::string& filePath) const;

    std::vector<std::string> GetSupportedFormats() const;
};