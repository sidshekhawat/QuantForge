#pragma once

#include <cstdint>

namespace quantforge::market {

enum class TimeframeUnit {
    Tick,
    Second,
    Minute,
    Hour,
    Day
};


class Timeframe {
public:
    using ValueType = std::uint32_t;

    constexpr Timeframe(
        ValueType value,
        TimeframeUnit unit
    ) noexcept
        : value_(value),
          unit_(unit) {}

    [[nodiscard]] constexpr ValueType value() const noexcept {
        return value_;
    }

    [[nodiscard]] constexpr TimeframeUnit unit() const noexcept {
        return unit_;
    }

    friend constexpr bool operator==(
        Timeframe lhs,
        Timeframe rhs
    ) noexcept {
        return lhs.value_ == rhs.value_
            && lhs.unit_ == rhs.unit_;
    }

    friend constexpr bool operator!=(
        Timeframe lhs,
        Timeframe rhs
    ) noexcept {
        return !(lhs == rhs);
    }

private:
    ValueType value_;
    TimeframeUnit unit_;
};

} // namespace quantforge::market