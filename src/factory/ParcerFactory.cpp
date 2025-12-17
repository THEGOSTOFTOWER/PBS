#include "ParcerFactory.h"

ParcerFactory::ParcerFactory() {
    RegisterParcer("pdf", []() { return std::make_unique<PdfParcerAdapter>(); });
    RegisterParcer("docx", []() { return std::make_unique<DocxParcerAdapter>(); });
    RegisterParcer("txt", []() { return std::make_unique<TextParcerAdapter>(); });
}

ParcerFactory& ParcerFactory::GetInstance() {
    static ParcerFactory instance;
    return instance;
}

void ParcerFactory::RegisterParcer(const std::string& format, ParcerCreator creator) {
    registeredParcers_[format] = std::move(creator);
}

std::unique_ptr<IDocumentParcer> ParcerFactory::CreateParcer(const std::string& format) const {
    std::string formatL(format.size(), '\0');
    std::transform(format.begin(), format.end(), formatL.begin(), ::tolower);
    auto it = registeredParcers_.find(formatL);
    if (it == registeredParcers_.end()) {
        throw UnsupportedFormatException(format);
    }

    return it->second();
}

std::unique_ptr<IDocumentParcer> ParcerFactory::CreateParcerForFile(const std::string& filePath) const {
    namespace fs = std::filesystem;

    fs::path path(filePath);
    std::string extension = path.extension().string();

    if (!extension.empty() && extension[0] == '.') {
        extension = extension.substr(1);
    }

    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return CreateParcer(extension);
}

std::vector<std::string> ParcerFactory::GetSupportedFormats() const {
    std::vector<std::string> formats;
    formats.reserve(registeredParcers_.size());

    for (const auto& pair : registeredParcers_) {
        formats.push_back(pair.first);
    }

    return formats;
}