#pragma once

#include "parsers/parser_factory.h"
#include "textfilters/text_filter.h"
#include "textfilters/filter_factory.h"
#include <functional>
#include <stdexcept>
#include <iostream>

class TDocumentProcessor {
public:
    using ProgressCallback = std::function<void(int)>;
    using TextFilter = std::function<std::string(const std::string&)>;

    explicit TDocumentProcessor(TParserFactory& factory);

    TDocumentInfo Process(const std::string& path, const std::vector<std::string>& filters = {}, ProgressCallback progress = [](int p) { std::cout << "Progress: " << p << "%\n"; });

private:
    TParserFactory& factory_;
};