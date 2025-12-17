#pragma once

#include <string>
#include <iterator>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>

class TTextFilter {
public:
    virtual ~TTextFilter() = default;
    virtual void Apply(const std::string& text) const = 0;
    virtual std::string GetName() const = 0;
};
