#include "quantforge/market/bar_validator.hpp"

namespace quantforge::market {

ValidationResult BarValidator::validate(const Bar& bar)
{
    if (bar.high() < bar.open()) {
        return ValidationResult::failure(
            "Bar high must be greater than or equal to open."
        );
    }

    if (bar.high() < bar.close()) {
        return ValidationResult::failure(
            "Bar high must be greater than or equal to close."
        );
    }

    if (bar.high() < bar.low()) {
        return ValidationResult::failure(
            "Bar high must be greater than or equal to low."
        );
    }

    if (bar.low() > bar.open()) {
        return ValidationResult::failure(
            "Bar low must be less than or equal to open."
        );
    }

    if (bar.low() > bar.close()) {
        return ValidationResult::failure(
            "Bar low must be less than or equal to close."
        );
    }

    if (bar.volume().value() < 0) {
        return ValidationResult::failure(
            "Bar volume must be non-negative."
        );
    }

    return ValidationResult::success();
}

}
