#include "text_filter.h"

class TUpperCaseFilter: public TTextFilter {
public:
    void Apply(const std::string& text) const override {
        std::string upperText = text;
        std::transform(upperText.begin(), upperText.end(), upperText.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        std::cout << "Upper case: " << upperText << std::endl;
    }

    std::string GetName() const override {
        return "TUpperCaseFilter";
    }
};