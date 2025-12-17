#include "TextFilter.h"
#include "WordCountFilter.h"
#include "LowerCaseFilter.h"
#include "UpperCaseFilter.h"
#include "RemovePunctuationFilter.h"
#include "exception/CustomException.h"
#include <memory>

struct FilterFactory {
    static std::shared_ptr<ITextFilter> CreateFilter(const std::string& type) {
        if (type.compare("lowercase") == 0) {
            return std::make_shared<LowerCaseFilter>();
        } else if (type.compare("uppercase") == 0) {
            return std::make_shared<UpperCaseFilter>();
        } else if (type.compare("nopunctuation") == 0) {
            return std::make_shared<RemovePunctuationFilter>();
        } else if (type.compare("wordcount") == 0) {
            return std::make_shared<WordCountFilter>();
        } else {
            throw FilterException("Uncnown Filter: " + type);
        }
    }

    static std::string GetSupportedFormats() {
        return (std::string("lowercase ") + std::string("uppercase ") + std::string("nopunctuation ") + std::string("wordcount"));
    }
};
