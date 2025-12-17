#pragma once

#include <string>
#include <iterator>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>

class ITextFilter {
public:
    virtual ~ITextFilter() = default;
    virtual void Apply(const std::string& text) const = 0;
    virtual std::string GetName() const = 0;
};
