#include "quantforge/market/bar_continuity_validator.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <vector>

namespace quantforge::market {

namespace {

Bar make_bar(
    InstrumentId instrument_id,
    Timestamp timestamp,
    Timeframe timeframe
)
{
    return Bar{
        instrument_id,
        timestamp,
        timeframe,
        Price{250000, 2},
        Price{251000, 2},
        Price{249000, 2},
        Price{250500, 2},
        Quantity{100000, 0}
    };
}

Timestamp timestamp_at_minutes(std::int64_t minutes)
{
    return Timestamp{
        std::chrono::sys_days{
            std::chrono::year{2026}
            / std::chrono::month{1}
            / std::chrono::day{2}
        }
        + std::chrono::hours{9}
        + std::chrono::minutes{minutes}
    };
}

} // namespace

TEST(BarContinuityValidatorTest, AcceptsEmptySequence)
{
    const std::vector<Bar> bars;

    const auto result =
        BarContinuityValidator::validate(bars);

    EXPECT_TRUE(result.valid());
}

TEST(BarContinuityValidatorTest, AcceptsSingleBar)
{
    const std::vector<Bar> bars{
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(15),
            Timeframe{5, TimeframeUnit::Minute}
        )
    };

    const auto result =
        BarContinuityValidator::validate(bars);

    EXPECT_TRUE(result.valid());
}

TEST(BarContinuityValidatorTest, AcceptsContinuousMinuteBars)
{
    const std::vector<Bar> bars{
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(15),
            Timeframe{5, TimeframeUnit::Minute}
        ),
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(20),
            Timeframe{5, TimeframeUnit::Minute}
        ),
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(25),
            Timeframe{5, TimeframeUnit::Minute}
        )
    };

    const auto result =
        BarContinuityValidator::validate(bars);

    EXPECT_TRUE(result.valid());
}

TEST(BarContinuityValidatorTest, RejectsTimestampGap)
{
    const std::vector<Bar> bars{
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(15),
            Timeframe{5, TimeframeUnit::Minute}
        ),
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(25),
            Timeframe{5, TimeframeUnit::Minute}
        )
    };

    const auto result =
        BarContinuityValidator::validate(bars);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Bar sequence contains a timestamp gap."
    );
}

TEST(BarContinuityValidatorTest, RejectsMultipleInstrumentIds)
{
    const std::vector<Bar> bars{
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(15),
            Timeframe{5, TimeframeUnit::Minute}
        ),
        make_bar(
            InstrumentId{99},
            timestamp_at_minutes(20),
            Timeframe{5, TimeframeUnit::Minute}
        )
    };

    const auto result =
        BarContinuityValidator::validate(bars);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Bars contain multiple instrument ids."
    );
}

TEST(BarContinuityValidatorTest, RejectsMultipleTimeframes)
{
    const std::vector<Bar> bars{
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(15),
            Timeframe{5, TimeframeUnit::Minute}
        ),
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(20),
            Timeframe{1, TimeframeUnit::Hour}
        )
    };

    const auto result =
        BarContinuityValidator::validate(bars);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Bars contain multiple timeframes."
    );
}

TEST(BarContinuityValidatorTest, AcceptsTickBarsWithoutTimestampContinuity)
{
    const std::vector<Bar> bars{
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(15),
            Timeframe{1, TimeframeUnit::Tick}
        ),
        make_bar(
            InstrumentId{42},
            timestamp_at_minutes(17),
            Timeframe{1, TimeframeUnit::Tick}
        )
    };

    const auto result =
        BarContinuityValidator::validate(bars);

    EXPECT_TRUE(result.valid());
}

} // namespace quantforge::market
