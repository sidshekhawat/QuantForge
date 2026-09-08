#pragma once

#include "quantforge/market/bar.hpp"
#include "quantforge/market/validation_result.hpp"

#include <vector>

namespace quantforge::market {

class BarContinuityValidator {
public:
    [[nodiscard]] static ValidationResult validate(
        const std::vector<Bar>& bars
    );
};

} // namespace quantforge::market
