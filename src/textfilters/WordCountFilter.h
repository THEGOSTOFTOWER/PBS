#include "TextFilter.h"

class WordCountFilter: public ITextFilter {
public:
    void Apply(const std::string& text) const override {
        int words = std::distance(
            std::istream_iterator<std::string>(
                std::istringstream(text) >> std::ws),
            std::istream_iterator<std::string>());
        std::cout << "[WordCountFilter] Words found: " << words << std::endl;
    }

    std::string GetName() const override {
        return "WordCountFilter";
    }
};