#pragma once

#include <cstdint>

namespace quantforge::market {

class InstrumentId {
public:
    using ValueType = std::uint64_t;

    constexpr explicit InstrumentId(ValueType value) noexcept
        : value_(value) {}

    [[nodiscard]] constexpr ValueType value() const noexcept {
        return value_;
    }

    friend constexpr bool operator==(
        InstrumentId lhs,
        InstrumentId rhs
    ) noexcept {
        return lhs.value_ == rhs.value_;
    }

    friend constexpr bool operator!=(
        InstrumentId lhs,
        InstrumentId rhs
    ) noexcept {
        return !(lhs == rhs);
    }

private:
    ValueType value_;
};

} // namespace quantforge::market