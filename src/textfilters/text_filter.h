#pragma once

#include <string>

class ITextFilter {
public:
    virtual ~ITextFilter() = default;
    virtual std::string Apply(const std::string& text) const = 0;
    virtual std::string GetName() const = 0;
};
