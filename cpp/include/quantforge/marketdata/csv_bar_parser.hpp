#pragma once

#include "quantforge/market/bar.hpp"

#include <cstddef>
#include <string_view>

namespace quantforge::marketdata {

class CsvBarParser {
public:
    [[nodiscard]] static market::Bar parse(
        std::string_view line,
        std::size_t row_number
    );
};

} // namespace quantforge::marketdata
