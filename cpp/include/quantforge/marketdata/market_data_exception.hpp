#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

namespace quantforge::marketdata {

class MarketDataException : public std::runtime_error {
public:
    MarketDataException(
        std::size_t row_number,
        std::string message
    )
        : std::runtime_error(
            "Market data error at row "
            + std::to_string(row_number)
            + ": "
            + message
        ),
          row_number_(row_number) {}

    [[nodiscard]] std::size_t row_number() const noexcept {
        return row_number_;
    }

private:
    std::size_t row_number_;
};

} // namespace quantforge::marketdata
