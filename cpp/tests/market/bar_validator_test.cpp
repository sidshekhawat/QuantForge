#include "quantforge/market/bar_validator.hpp"

#include <gtest/gtest.h>

namespace quantforge::market {

namespace {

Bar makeValidBar()
{
    return Bar{
        InstrumentId{1},
        Timestamp{},
        Timeframe{1, TimeframeUnit::Minute},
        Price{10000, 2},
        Price{10500, 2},
        Price{9900, 2},
        Price{10300, 2},
        Quantity{1000, 0}
    };
}

} // namespace

TEST(BarValidatorTest, AcceptsValidBar)
{
    const Bar bar = makeValidBar();

    const auto result = BarValidator::validate(bar);

    EXPECT_TRUE(result.valid());
    EXPECT_TRUE(result.message().empty());
}

TEST(BarValidatorTest, RejectsHighBelowOpen)
{
    Bar bar = makeValidBar();

    bar = Bar{
        bar.instrument_id(),
        bar.timestamp(),
        bar.timeframe(),
        bar.open(),
        Price{9950, 2},
        bar.low(),
        bar.close(),
        bar.volume()
    };

    const auto result = BarValidator::validate(bar);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Bar high must be greater than or equal to open."
    );
}

TEST(BarValidatorTest, RejectsHighBelowClose)
{
    Bar bar = makeValidBar();

    bar = Bar{
        bar.instrument_id(),
        bar.timestamp(),
        bar.timeframe(),
        bar.open(),
        Price{10200, 2},
        bar.low(),
        Price{10300, 2},
        bar.volume()
    };

    const auto result = BarValidator::validate(bar);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Bar high must be greater than or equal to close."
    );
}

TEST(BarValidatorTest, RejectsHighBelowLow)
{
    Bar bar = makeValidBar();

    bar = Bar{
        bar.instrument_id(),
        bar.timestamp(),
        bar.timeframe(),
        Price{10000, 2},
        Price{10100, 2},
        Price{10200, 2},
        Price{10000, 2},
        bar.volume()
    };
    const auto result = BarValidator::validate(bar);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Bar high must be greater than or equal to low."
    );
}

TEST(BarValidatorTest, RejectsLowAboveOpen)
{
    Bar bar = makeValidBar();

    bar = Bar{
        bar.instrument_id(),
        bar.timestamp(),
        bar.timeframe(),
        bar.open(),
        bar.high(),
        Price{10100, 2},
        bar.close(),
        bar.volume()
    };

    const auto result = BarValidator::validate(bar);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Bar low must be less than or equal to open."
    );
}

TEST(BarValidatorTest, RejectsLowAboveClose)
{
    Bar bar = makeValidBar();

    bar = Bar{
        bar.instrument_id(),
        bar.timestamp(),
        bar.timeframe(),
        Price{10500, 2},
        Price{10600, 2},
        Price{10400, 2},
        Price{10300, 2},
        bar.volume()
    };

    const auto result = BarValidator::validate(bar);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Bar low must be less than or equal to close."
    );
}

TEST(BarValidatorTest, RejectsNegativeVolume)
{
    Bar bar = makeValidBar();

    bar = Bar{
        bar.instrument_id(),
        bar.timestamp(),
        bar.timeframe(),
        bar.open(),
        bar.high(),
        bar.low(),
        bar.close(),
        Quantity{-1, 0}
    };

    const auto result = BarValidator::validate(bar);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Bar volume must be non-negative."
    );
}

TEST(BarValidatorTest, AcceptsEqualHighAndOpen)
{
    Bar bar = makeValidBar();

    bar = Bar{
        bar.instrument_id(),
        bar.timestamp(),
        bar.timeframe(),
        bar.open(),
        bar.open(),
        bar.low(),
        bar.open(),
        bar.volume()
    };

    const auto result = BarValidator::validate(bar);

    EXPECT_TRUE(result.valid());
}

TEST(BarValidatorTest, AcceptsEqualLowAndOpen)
{
    Bar bar = makeValidBar();

    bar = Bar{
        bar.instrument_id(),
        bar.timestamp(),
        bar.timeframe(),
        bar.open(),
        bar.high(),
        bar.open(),
        bar.close(),
        bar.volume()
    };

    const auto result = BarValidator::validate(bar);

    EXPECT_TRUE(result.valid());
}

TEST(BarValidatorTest, AcceptsZeroVolume)
{
    Bar bar = makeValidBar();

    bar = Bar{
        bar.instrument_id(),
        bar.timestamp(),
        bar.timeframe(),
        bar.open(),
        bar.high(),
        bar.low(),
        bar.close(),
        Quantity{0, 0}
    };

    const auto result = BarValidator::validate(bar);

    EXPECT_TRUE(result.valid());
}

}