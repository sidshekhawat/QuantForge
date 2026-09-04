#pragma once

#include "quantforge/market/bar.hpp"
#include "quantforge/market/validation_result.hpp"

namespace quantforge::market {

class BarValidator {
public:
    [[nodiscard]] static ValidationResult validate(const Bar& bar);
};

}
