#include "quantforge/backtest/backtest_clock.hpp"

#include <chrono>
#include <stdexcept>

#include <gtest/gtest.h>

namespace {

using quantforge::backtest::BacktestClock;
using quantforge::market::Timestamp;

TEST(BacktestClockTest, StartsAtProvidedTimestamp)
{
    const auto start_time =
        Timestamp{
            std::chrono::seconds{100}
        };

    const BacktestClock clock{start_time};

    EXPECT_EQ(clock.now(), start_time);
}

TEST(BacktestClockTest, AdvancesForward)
{
    const auto start_time =
        Timestamp{
            std::chrono::seconds{100}
        };

    const auto next_time =
        Timestamp{
            std::chrono::seconds{160}
        };

    BacktestClock clock{start_time};

    clock.advance_to(next_time);

    EXPECT_EQ(clock.now(), next_time);
}

TEST(BacktestClockTest, AllowsAdvancingToSameTimestamp)
{
    const auto timestamp =
        Timestamp{
            std::chrono::seconds{100}
        };

    BacktestClock clock{timestamp};

    EXPECT_NO_THROW(clock.advance_to(timestamp));
    EXPECT_EQ(clock.now(), timestamp);
}

TEST(BacktestClockTest, RejectsMovingBackwards)
{
    const auto start_time =
        Timestamp{
            std::chrono::seconds{100}
        };

    const auto earlier_time =
        Timestamp{
            std::chrono::seconds{99}
        };

    BacktestClock clock{start_time};

    EXPECT_THROW(
        clock.advance_to(earlier_time),
        std::logic_error
    );

    EXPECT_EQ(clock.now(), start_time);
}

TEST(BacktestClockTest, PreservesCurrentTimeAfterRejectedAdvance)
{
    const auto start_time =
        Timestamp{
            std::chrono::seconds{100}
        };

    const auto earlier_time =
        Timestamp{
            std::chrono::seconds{50}
        };

    BacktestClock clock{start_time};

    try {
        clock.advance_to(earlier_time);
    } catch (const std::logic_error&) {
    }

    EXPECT_EQ(clock.now(), start_time);
}

TEST(BacktestClockTest, SupportsNanosecondPrecision)
{
    const auto start_time =
        Timestamp{
            std::chrono::nanoseconds{1000000000}
        };

    const auto next_time =
        Timestamp{
            std::chrono::nanoseconds{1000000001}
        };

    BacktestClock clock{start_time};

    clock.advance_to(next_time);

    EXPECT_EQ(clock.now(), next_time);
}

} // namespace
