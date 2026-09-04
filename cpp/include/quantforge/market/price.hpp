#pragma once

#include <cstdint>

namespace quantforge::market {

class Price {
public:
    using ValueType = std::int64_t;
    using ScaleType = std::uint8_t;

    constexpr Price(ValueType value, ScaleType scale) noexcept
        : value_(value), scale_(scale) {}

    [[nodiscard]] constexpr ValueType value() const noexcept {
        return value_;
    }

    [[nodiscard]] constexpr ScaleType scale() const noexcept {
        return scale_;
    }

    friend constexpr bool operator==(
        Price lhs,
        Price rhs
    ) noexcept {
        if (lhs.scale_ == rhs.scale_) {
            return lhs.value_ == rhs.value_;
        }

        if (lhs.scale_ > rhs.scale_) {
            return lhs.value_ ==
                rhs.value_ * pow10(lhs.scale_ - rhs.scale_);
        }

        return lhs.value_ * pow10(rhs.scale_ - lhs.scale_) ==
            rhs.value_;
    }

    friend constexpr bool operator!=(
        Price lhs,
        Price rhs
    ) noexcept {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(
        Price lhs,
        Price rhs
    ) noexcept {
        if (lhs.scale_ == rhs.scale_) {
            return lhs.value_ < rhs.value_;
        }

        if (lhs.scale_ > rhs.scale_) {
            return lhs.value_ <
                rhs.value_ * pow10(lhs.scale_ - rhs.scale_);
        }

        return lhs.value_ * pow10(rhs.scale_ - lhs.scale_) <
            rhs.value_;
    }

    friend constexpr bool operator>(
        Price lhs,
        Price rhs
    ) noexcept {
        return rhs < lhs;
    }

    friend constexpr bool operator<=(
        Price lhs,
        Price rhs
    ) noexcept {
        return !(rhs < lhs);
    }

    friend constexpr bool operator>=(
        Price lhs,
        Price rhs
    ) noexcept {
        return !(lhs < rhs);
    }

private:
    static constexpr std::int64_t pow10(
        std::uint8_t exponent
    ) noexcept {
        std::int64_t result = 1;

        for (
            std::uint8_t i = 0;
            i < exponent;
            ++i
        ) {
            result *= 10;
        }

        return result;
    }

    ValueType value_;
    ScaleType scale_;
};

} // namespace quantforge::market