#include "quantforge/market/timeframe.hpp"

#include <charconv>
#include <stdexcept>
#include <string>

namespace quantforge::market {

Timeframe parse_timeframe(std::string_view value)
{
    if (value.size() < 2) {
        throw std::invalid_argument("Invalid timeframe: " + std::string{value});
    }

    const char unit = value.back();
    const auto number = value.substr(0, value.size() - 1);

    if (number.empty()) {
        throw std::invalid_argument(
            "Invalid timeframe: " + std::string{value}
        );
    }

    Timeframe::ValueType parsed_value{};

    const auto [ptr, error] = std::from_chars(
        number.data(),
        number.data() + number.size(),
        parsed_value
    );

    if (
        error != std::errc{}
        || ptr != number.data() + number.size()
        || parsed_value == 0
    ) {
        throw std::invalid_argument(
            "Invalid timeframe: " + std::string{value}
        );
    }

    TimeframeUnit parsed_unit;

    switch (unit) {
        case 't':
            parsed_unit = TimeframeUnit::Tick;
            break;

        case 's':
            parsed_unit = TimeframeUnit::Second;
            break;

        case 'm':
            parsed_unit = TimeframeUnit::Minute;
            break;

        case 'h':
            parsed_unit = TimeframeUnit::Hour;
            break;

        case 'd':
            parsed_unit = TimeframeUnit::Day;
            break;

        default:
            throw std::invalid_argument(
                "Invalid timeframe: " + std::string{value}
            );
    }

    return Timeframe{
        parsed_value,
        parsed_unit
    };
}

} // namespace quantforge::market
