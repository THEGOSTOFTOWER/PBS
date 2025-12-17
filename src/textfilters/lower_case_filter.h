#include "text_filter.h"

class TLowerCaseFilter: public TTextFilter {
public:
    void Apply(const std::string& text) const override {
        std::string lowerText = text;
        std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        std::cout << "Lower case: " << lowerText << std::endl;
    }

    std::string GetName() const override {
        return "TLowerCaseFilter";
    }
};