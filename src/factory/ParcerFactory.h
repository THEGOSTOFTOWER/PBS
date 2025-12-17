#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cctype>
#include "adapter/IDocumentParcer.h"
#include "adapter/PdfParcerAdapter.h"
#include "adapter/DocxParcerAdapter.h"
#include "adapter/TextParcerAdapter.h"

class ParcerFactory {
private:
    using ParcerCreator = std::function<std::unique_ptr<IDocumentParcer>()>;
    std::unordered_map<std::string, ParcerCreator> registeredParcers_;

    ParcerFactory();

public:
    static ParcerFactory& GetInstance();

    void RegisterParcer(const std::string& format, ParcerCreator creator);

    std::unique_ptr<IDocumentParcer> CreateParcer(const std::string& format) const;

    std::unique_ptr<IDocumentParcer> CreateParcerForFile(const std::string& filePath) const;

    std::vector<std::string> GetSupportedFormats() const;
};