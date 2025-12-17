#pragma once

#include <stdexcept>
#include <string>

class TDocumentProcessingException: public std::runtime_error {
public:
    explicit TDocumentProcessingException(const std::string& message)
        : std::runtime_error(message) {
    }
};

class TUnsupportedFormatException: public TDocumentProcessingException {
public:
    explicit TUnsupportedFormatException(const std::string& format)
        : TDocumentProcessingException("Unsupported format: " + format) {
    }
};

class TParsingException: public TDocumentProcessingException {
public:
    explicit TParsingException(const std::string& filename, const std::string& error)
        : TDocumentProcessingException("Failed to parse '" + filename + "': " + error) {
    }
};

class FilterException: public std::runtime_error {
public:
    explicit FilterException(const std::string& message)
        : std::runtime_error(message) {
    }
};