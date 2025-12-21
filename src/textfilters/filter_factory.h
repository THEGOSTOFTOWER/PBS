#pragma once

#include "text_filter.h"
#include "word_count_filter.h"
#include "lower_case_filter.h"
#include "upper_case_filter.h"
#include "remove_punctuation_filter.h"
#include "exception/custom_exceptions.h"

#include <memory>

struct TFilterFactory {
    static std::shared_ptr<ITextFilter> CreateFilter(const std::string& type) {
        if (type.compare("lowercase") == 0) {
            return std::make_shared<TLowerCaseFilter>();
        } else if (type.compare("uppercase") == 0) {
            return std::make_shared<TUpperCaseFilter>();
        } else if (type.compare("nopunctuation") == 0) {
            return std::make_shared<TRemovePunctuationFilter>();
        } else if (type.compare("wordcount") == 0) {
            return std::make_shared<TWordCountFilter>();
        } else {
            throw FilterException("Unknown Filter: " + type);
        }
    }

    static std::string GetSupportedFormats() {
        return (std::string("lowercase ") + std::string("uppercase ") + std::string("nopunctuation ") + std::string("wordcount"));
    }
};
