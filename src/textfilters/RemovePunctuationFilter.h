#include "TextFilter.h"

class RemovePunctuationFilter: public ITextFilter {
public:
    void Apply(const std::string& text) const override {
        std::string noPunctText = text;
        noPunctText.erase(std::remove_if(noPunctText.begin(), noPunctText.end(),
                                         [](unsigned char c) { return std::ispunct(c); }),
                          noPunctText.end());
        std::cout << "No punctuation: " << noPunctText << std::endl;
    }

    std::string GetName() const override {
        return "RemovePunctuationFilter";
    }
};