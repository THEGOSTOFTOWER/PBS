#include "parser_factory.h"

TParserFactory::TParserFactory() {
    RegisterParser("pdf", []() { return std::make_unique<TPdfParserAdapter>(); });
    RegisterParser("docx", []() { return std::make_unique<TDocxParserAdapter>(); });
    RegisterParser("txt", []() { return std::make_unique<TTextParserAdapter>(); });
}

TParserFactory& TParserFactory::GetInstance() {
    static TParserFactory instance;
    return instance;
}

void TParserFactory::RegisterParser(const std::string& format, ParserCreator creator) {
    registeredParsers_[format] = std::move(creator);
}

std::unique_ptr<TIDocumentParser> TParserFactory::CreateParser(const std::string& format) const {
    std::string formatL(format.size(), '\0');
    std::transform(format.begin(), format.end(), formatL.begin(), ::tolower);
    auto it = registeredParsers_.find(formatL);
    if (it == registeredParsers_.end()) {
        throw TUnsupportedFormatException(format);
    }

    return it->second();
}

std::unique_ptr<TIDocumentParser> TParserFactory::CreateParserForFile(const std::string& filePath) const {
    namespace fs = std::filesystem;

    fs::path path(filePath);
    std::string extension = path.extension().string();

    if (!extension.empty() && extension[0] == '.') {
        extension = extension.substr(1);
    }

    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return CreateParser(extension);
}

std::vector<std::string> TParserFactory::GetSupportedFormats() const {
    std::vector<std::string> formats;
    formats.reserve(registeredParsers_.size());

    for (const auto& pair : registeredParsers_) {
        formats.push_back(pair.first);
    }

    return formats;
}