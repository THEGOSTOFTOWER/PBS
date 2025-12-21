#pragma once

#include "text_filter.h"

#include <iterator>
#include <algorithm>

class TRemovePunctuationFilter: public ITextFilter {
public:
    std::string Apply(const std::string& text) const override {
        std::string noPunctText = text;
        noPunctText.erase(std::remove_if(noPunctText.begin(), noPunctText.end(),
                                         [](unsigned char c) { return std::ispunct(c); }),
                          noPunctText.end());
        return "No punctuation: " + noPunctText;
    }

    std::string GetName() const override {
        return "TRemovePunctuationFilter";
    }
};