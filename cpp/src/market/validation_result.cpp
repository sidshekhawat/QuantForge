#include "quantforge/market/validation_result.hpp"

namespace quantforge::market {

ValidationResult::ValidationResult(
    bool valid,
    std::string message
)
    : valid_(valid),
      message_(std::move(message))
{
}


ValidationResult ValidationResult::success()
{
    return ValidationResult(true, "");
}


ValidationResult ValidationResult::failure(
    std::string message
)
{
    return ValidationResult(false, std::move(message));
}


bool ValidationResult::valid() const
{
    return valid_;
}


const std::string& ValidationResult::message() const
{
    return message_;
}

}