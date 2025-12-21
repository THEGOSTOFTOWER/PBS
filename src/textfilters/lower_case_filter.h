#pragma once

#include "text_filter.h"

#include <iterator>
#include <algorithm>

class TLowerCaseFilter: public ITextFilter {
public:
    std::string Apply(const std::string& text) const override {
        std::string lowerText = text;
        std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return "Lower case: " + lowerText;
    }

    std::string GetName() const override {
        return "TLowerCaseFilter";
    }
};