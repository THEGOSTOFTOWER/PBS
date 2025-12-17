#pragma once

#include "factory/ParcerFactory.h"
#include "textfilters/TextFilter.h"
#include "textfilters/TextFilterFactory.h"
#include <functional>
#include <stdexcept>
#include <iostream>

class DocumentProcessor {
public:
    using ProgressCallback = std::function<void(int)>;
    using TextFilter = std::function<std::string(const std::string&)>;

    explicit DocumentProcessor(ParcerFactory& factory);

    DocumentInfo Process(const std::string& path, const std::vector<std::string>& filters = {}, ProgressCallback progress = [](int p) { std::cout << "Progress: " << p << "%\n"; });

private:
    ParcerFactory& factory_;
};