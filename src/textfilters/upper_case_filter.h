#pragma once

#include "text_filter.h"

#include <iterator>
#include <algorithm>

class TUpperCaseFilter: public ITextFilter {
public:
    std::string Apply(const std::string& text) const override {
        std::string upperText = text;
        std::transform(upperText.begin(), upperText.end(), upperText.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        std::string res = "Upper case: " + upperText;
        return res;
    }

    std::string GetName() const override {
        return "TUpperCaseFilter";
    }
};