#pragma once

#include "text_filter.h"

#include <iterator>
#include <sstream>
#include <algorithm>

class TWordCountFilter: public ITextFilter {
public:
    std::string Apply(const std::string& text) const override {
        int words = std::distance(
            std::istream_iterator<std::string>(
                std::istringstream(text) >> std::ws),
            std::istream_iterator<std::string>());
        std::string res = "[WordCount] Words found: " + std::to_string(words);
        return res;
    }

    std::string GetName() const override {
        return "WordCount";
    }
};