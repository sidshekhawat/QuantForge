#include <gtest/gtest.h>

#include <stdexcept>

#include "quantforge/market/timeframe.hpp"

namespace quantforge::market {

TEST(TimeframeTest, StoresValue)
{
    Timeframe timeframe{
        5,
        TimeframeUnit::Minute
    };

    EXPECT_EQ(timeframe.value(), 5);
}

TEST(TimeframeTest, StoresUnit)
{
    Timeframe timeframe{
        5,
        TimeframeUnit::Minute
    };

    EXPECT_EQ(
        timeframe.unit(),
        TimeframeUnit::Minute
    );
}

TEST(TimeframeTest, ComparesEqualTimeframes)
{
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

TEST(TimeframeTest, ComparesDifferentTimeframes)
{
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

TEST(TimeframeTest, DifferentUnitsAreDifferent)
{
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

TEST(TimeframeTest, ParsesTick)
{
    EXPECT_EQ(
        parse_timeframe("1t"),
        (Timeframe{1, TimeframeUnit::Tick})
    );
}

TEST(TimeframeTest, ParsesSeconds)
{
    EXPECT_EQ(
        parse_timeframe("5s"),
        (Timeframe{5, TimeframeUnit::Second})
    );
}

TEST(TimeframeTest, ParsesMinutes)
{
    EXPECT_EQ(
        parse_timeframe("15m"),
        (Timeframe{15, TimeframeUnit::Minute})
    );
}

TEST(TimeframeTest, ParsesHours)
{
    EXPECT_EQ(
        parse_timeframe("2h"),
        (Timeframe{2, TimeframeUnit::Hour})
    );
}

TEST(TimeframeTest, ParsesDays)
{
    EXPECT_EQ(
        parse_timeframe("1d"),
        (Timeframe{1, TimeframeUnit::Day})
    );
}

TEST(TimeframeTest, RejectsEmptyString)
{
    EXPECT_THROW(
        static_cast<void>(parse_timeframe("")),
        std::invalid_argument
    );
}

TEST(TimeframeTest, RejectsMissingNumber)
{
    EXPECT_THROW(
        static_cast<void>(parse_timeframe("m")),
        std::invalid_argument
    );
}

TEST(TimeframeTest, RejectsMissingUnit)
{
    EXPECT_THROW(
        static_cast<void>(parse_timeframe("15")),
        std::invalid_argument
    );
}

TEST(TimeframeTest, RejectsUnknownUnit)
{
    EXPECT_THROW(
        static_cast<void>(parse_timeframe("15w")),
        std::invalid_argument
    );
}

TEST(TimeframeTest, RejectsZeroValue)
{
    EXPECT_THROW(
        static_cast<void>(parse_timeframe("0m")),
        std::invalid_argument
    );
}

TEST(TimeframeTest, RejectsNonNumericValue)
{
    EXPECT_THROW(
        static_cast<void>(parse_timeframe("xm")),
        std::invalid_argument
    );
}

}
