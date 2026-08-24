#include <chrono>

#include <gtest/gtest.h>

#include "quantforge/market/timestamp.hpp"

namespace quantforge::market {

TEST(TimestampTest, StoresNanosecondPrecision) {
    const Timestamp timestamp{
        std::chrono::nanoseconds{123456789}
    };

    EXPECT_EQ(
        timestamp.time_since_epoch(),
        std::chrono::nanoseconds{123456789}
    );
}

TEST(TimestampTest, SupportsOrdering) {
    const Timestamp earlier{
        std::chrono::nanoseconds{100}
    };

    const Timestamp later{
        std::chrono::nanoseconds{200}
    };

    EXPECT_LT(earlier, later);
    EXPECT_GT(later, earlier);
}

TEST(TimestampTest, SupportsEquality) {
    const Timestamp first{
        std::chrono::nanoseconds{123456789}
    };

    const Timestamp second{
        std::chrono::nanoseconds{123456789}
    };

    EXPECT_EQ(first, second);
}

} // namespace quantforge::market