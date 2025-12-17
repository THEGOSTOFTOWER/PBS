#pragma once

#include <stdexcept>
#include <string>

class DocumentProcessingException: public std::runtime_error {
public:
    explicit DocumentProcessingException(const std::string& message)
        : std::runtime_error(message) {
    }
};

class UnsupportedFormatException: public DocumentProcessingException {
public:
    explicit UnsupportedFormatException(const std::string& format)
        : DocumentProcessingException("Unsupported format: " + format) {
    }
};

class ParsingException: public DocumentProcessingException {
public:
    explicit ParsingException(const std::string& filename, const std::string& error)
        : DocumentProcessingException("Failed to parse '" + filename + "': " + error) {
    }
};

class FilterException: public std::runtime_error {
public:
    explicit FilterException(const std::string& message)
        : std::runtime_error(message) {
    }
};