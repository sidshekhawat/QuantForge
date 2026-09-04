#pragma once

#include <string>

namespace quantforge::market {

class ValidationResult {
public:
    static ValidationResult success();

    static ValidationResult failure(std::string message);

    bool valid() const;

    const std::string& message() const;

private:
    ValidationResult(bool valid, std::string message);

    bool valid_;
    std::string message_;
};

}