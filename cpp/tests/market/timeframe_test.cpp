#include <gtest/gtest.h>

#include "quantforge/market/timeframe.hpp"

namespace quantforge::market {

TEST(TimeframeTest, StoresValue) {
    Timeframe timeframe{
        5,
        TimeframeUnit::Minute
    };

    EXPECT_EQ(timeframe.value(), 5);
}


TEST(TimeframeTest, StoresUnit) {
    Timeframe timeframe{
        5,
        TimeframeUnit::Minute
    };

    EXPECT_EQ(
        timeframe.unit(),
        TimeframeUnit::Minute
    );
}


TEST(TimeframeTest, ComparesEqualTimeframes) {
    Timeframe first{
        5,
        TimeframeUnit::Minute
    };

    Timeframe second{
        5,
        TimeframeUnit::Minute
    };

    EXPECT_EQ(first, second);
}


TEST(TimeframeTest, ComparesDifferentTimeframes) {
    Timeframe first{
        5,
        TimeframeUnit::Minute
    };

    Timeframe second{
        15,
        TimeframeUnit::Minute
    };

    EXPECT_NE(first, second);
}


TEST(TimeframeTest, DifferentUnitsAreDifferent) {
    Timeframe first{
        1,
        TimeframeUnit::Hour
    };

    Timeframe second{
        60,
        TimeframeUnit::Minute
    };

    EXPECT_NE(first, second);
}


} // namespace quantforge::market